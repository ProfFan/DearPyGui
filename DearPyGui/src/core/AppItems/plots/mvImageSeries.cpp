#include <utility>
#include "mvImageSeries.h"
#include "mvCore.h"
#include "mvApp.h"
#include "mvItemRegistry.h"
#include "mvPythonExceptions.h"
#include "AppItems/fonts/mvFont.h"
#include "AppItems/themes/mvTheme.h"
#include "AppItems/containers/mvDragPayload.h"

namespace Marvel {

	void mvImageSeries::InsertParser(std::map<std::string, mvPythonParser>* parsers)
	{

		mvPythonParser parser(mvPyDataType::UUID, "Adds a image series to a plot.", { "Plotting", "Containers", "Widgets", "Textures"});
		mvAppItem::AddCommonArgs(parser, (CommonParserArgs)(
			MV_PARSER_ARG_ID |
			MV_PARSER_ARG_PARENT |
			MV_PARSER_ARG_BEFORE |
			MV_PARSER_ARG_SOURCE |
			MV_PARSER_ARG_SHOW)
		);

		parser.addArg<mvPyDataType::UUID>("texture_id");

		parser.addArg<mvPyDataType::DoubleList>("bounds_min");
		parser.addArg<mvPyDataType::DoubleList>("bounds_max");

		parser.addArg<mvPyDataType::FloatList>("uv_min", mvArgType::KEYWORD_ARG, "(0.0, 0.0)", "normalized texture coordinates");
		parser.addArg<mvPyDataType::FloatList>("uv_max", mvArgType::KEYWORD_ARG, "(1.0, 1.0)", "normalized texture coordinates");

		parser.addArg<mvPyDataType::IntList>("tint_color", mvArgType::KEYWORD_ARG, "(255, 255, 255, 255)");

		parser.finalize();

		parsers->insert({ s_command, parser });
	}

	mvImageSeries::mvImageSeries(mvUUID uuid)
		: mvSeriesBase(uuid)
	{
	}

	bool mvImageSeries::isParentCompatible(mvAppItemType type)
	{
		if (type == mvAppItemType::mvPlotAxis) return true;

		mvThrowPythonError(mvErrorCode::mvIncompatibleParent, s_command,
			"Incompatible parent. Acceptable parents include: plot axis", this);

		assert(false);
		return false;
	}

	void mvImageSeries::draw(ImDrawList* drawlist, float x, float y)
	{

		//-----------------------------------------------------------------------------
		// pre draw
		//-----------------------------------------------------------------------------
		if (!_show)
			return;

		// push font if a font object is attached
		if (_font)
		{
			ImFont* fontptr = static_cast<mvFont*>(_font.get())->getFontPtr();
			ImGui::PushFont(fontptr);
		}

		// handle enabled theming
		if (_enabled)
		{
			// push class theme (if it exists)
			if (auto classTheme = getClassTheme())
				static_cast<mvTheme*>(classTheme.get())->draw(nullptr, 0.0f, 0.0f);

			// push item theme (if it exists)
			if (_theme)
				static_cast<mvTheme*>(_theme.get())->draw(nullptr, 0.0f, 0.0f);
		}

		//-----------------------------------------------------------------------------
		// draw
		//-----------------------------------------------------------------------------
		{

			if (_texture)
			{
				if (_internalTexture)
					_texture->draw(drawlist, x, y);

				if (!_texture->getState().isOk())
					return;

				void* texture = nullptr;

				if (_texture->getType() == mvAppItemType::mvStaticTexture)
					texture = static_cast<mvStaticTexture*>(_texture.get())->getRawTexture();
				else if (_texture->getType() == mvAppItemType::mvRawTexture)
					texture = static_cast<mvRawTexture*>(_texture.get())->getRawTexture();
				else
					texture = static_cast<mvDynamicTexture*>(_texture.get())->getRawTexture();

				ImPlot::PlotImage(_label.c_str(), texture, _bounds_min, _bounds_max, _uv_min, _uv_max, _tintColor);

				// Begin a popup for a legend entry.
				if (ImPlot::BeginLegendPopup(_label.c_str(), 1))
				{
					for (auto& childset : _children)
					{
						for (auto& item : childset)
						{
							// skip item if it's not shown
							if (!item->isShown())
								continue;
							item->draw(drawlist, ImPlot::GetPlotPos().x, ImPlot::GetPlotPos().y);
							item->getState().update();
						}
					}
					ImPlot::EndLegendPopup();
				}
			}
		}

		//-----------------------------------------------------------------------------
		// update state
		//   * only update if applicable
		//-----------------------------------------------------------------------------


		//-----------------------------------------------------------------------------
		// post draw
		//-----------------------------------------------------------------------------

		// pop font off stack
		if (_font)
			ImGui::PopFont();

		// handle popping styles
		if (_enabled)
		{
			if (auto classTheme = getClassTheme())
				static_cast<mvTheme*>(classTheme.get())->customAction();

			if (_theme)
				static_cast<mvTheme*>(_theme.get())->customAction();
		}

	}

	void mvImageSeries::handleSpecificRequiredArgs(PyObject* dict)
	{
		if (!mvApp::GetApp()->getParsers()[s_command].verifyRequiredArguments(dict))
			return;

		for (int i = 0; i < PyTuple_Size(dict); i++)
		{
			PyObject* item = PyTuple_GetItem(dict, i);
			switch (i)
			{
			case 0:
			{
				_textureUUID = ToUUID(item);
				_texture = mvApp::GetApp()->getItemRegistry().getRefItem(_textureUUID);
				if (_texture)
					break;
				else if (_textureUUID == MV_ATLAS_UUID)
				{
					_texture = std::make_shared<mvStaticTexture>(_textureUUID);
					_internalTexture = true;
					break;
				}
				else
				{
					mvThrowPythonError(mvErrorCode::mvTextureNotFound, s_command, "Texture not found.", this);
					break;
				}
			}

			case 1:
			{
				auto result = ToPoint(item);
				_bounds_min.x = result.x;
				_bounds_min.y = result.y;
				break;
			}

			case 2:
			{
				auto result = ToPoint(item);
				_bounds_max.x = result.x;
				_bounds_max.y = result.y;
				break;
			}

			default:
				break;
			}
		}
	}

	void mvImageSeries::handleSpecificKeywordArgs(PyObject* dict)
	{
		if (dict == nullptr)
			return;

		if (PyObject* item = PyDict_GetItemString(dict, "uv_min")) _uv_min = ToVec2(item);
		if (PyObject* item = PyDict_GetItemString(dict, "uv_max")) _uv_max = ToVec2(item);
		if (PyObject* item = PyDict_GetItemString(dict, "tint_color")) _tintColor = ToColor(item);
		if (PyObject* item = PyDict_GetItemString(dict, "bounds_min"))
		{
			auto result = ToPoint(item);
			_bounds_min.x = result.x;
			_bounds_min.y = result.y;
		}
		if (PyObject* item = PyDict_GetItemString(dict, "bounds_max"))
		{
			auto result = ToPoint(item);
			_bounds_max.x = result.x;
			_bounds_max.y = result.y;
		}

	}

	void mvImageSeries::getSpecificConfiguration(PyObject* dict)
	{
		if (dict == nullptr)
			return;

		PyDict_SetItemString(dict, "texture_id", ToPyUUID(_textureUUID));
		PyDict_SetItemString(dict, "uv_min", ToPyPair(_uv_min.x, _uv_min.y));
		PyDict_SetItemString(dict, "uv_max", ToPyPair(_uv_max.x, _uv_max.y));
		PyDict_SetItemString(dict, "tint_color", ToPyColor(_tintColor));
		PyDict_SetItemString(dict, "bounds_min", ToPyPair(_bounds_min.x, _bounds_min.y));
		PyDict_SetItemString(dict, "bounds_max", ToPyPair(_bounds_max.x, _bounds_max.y));
	}

}