#include <utility>
#include "mvShadeSeries.h"
#include "mvCore.h"
#include "mvApp.h"
#include "mvItemRegistry.h"
#include "mvPythonExceptions.h"
#include "AppItems/fonts/mvFont.h"
#include "AppItems/themes/mvTheme.h"
#include "AppItems/containers/mvDragPayload.h"

namespace Marvel {

	void mvShadeSeries::InsertParser(std::map<std::string, mvPythonParser>* parsers)
	{

		mvPythonParser parser(mvPyDataType::UUID, "Adds a shade series to a plot.", { "Plotting", "Containers", "Widgets" });
		mvAppItem::AddCommonArgs(parser, (CommonParserArgs)(
			MV_PARSER_ARG_ID |
			MV_PARSER_ARG_PARENT |
			MV_PARSER_ARG_BEFORE |
			MV_PARSER_ARG_SOURCE |
			MV_PARSER_ARG_SHOW)
		);

		parser.addArg<mvPyDataType::DoubleList>("x");
		parser.addArg<mvPyDataType::DoubleList>("y1");

		parser.addArg<mvPyDataType::DoubleList>("y2", mvArgType::KEYWORD_ARG, "[]");

		parser.finalize();

		parsers->insert({ s_command, parser });
	}

	mvShadeSeries::mvShadeSeries(mvUUID uuid)
		: mvSeriesBase(uuid)
	{
	}

	bool mvShadeSeries::isParentCompatible(mvAppItemType type)
	{
		if (type == mvAppItemType::mvPlotAxis) return true;

		mvThrowPythonError(mvErrorCode::mvIncompatibleParent, s_command,
			"Incompatible parent. Acceptable parents include: plot axis", this);

		assert(false);
		return false;
	}

	void mvShadeSeries::draw(ImDrawList* drawlist, float x, float y)
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

			static const std::vector<double>* xptr;
			static const std::vector<double>* y1ptr;
			static const std::vector<double>* y2ptr;

			xptr = &(*_value.get())[0];
			y1ptr = &(*_value.get())[1];
			y2ptr = &(*_value.get())[2];

			ImPlot::PlotShaded(_label.c_str(), xptr->data(), y1ptr->data(),
				y2ptr->data(), (int)xptr->size());

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

		//-----------------------------------------------------------------------------
		// update state
		//   * only update if applicable
		//-----------------------------------------------------------------------------


		//-----------------------------------------------------------------------------
		// post draw
		//-----------------------------------------------------------------------------
		// 
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

	void mvShadeSeries::handleSpecificRequiredArgs(PyObject* dict)
	{
		if (!mvApp::GetApp()->getParsers()[s_command].verifyRequiredArguments(dict))
			return;

		for (int i = 0; i < PyTuple_Size(dict); i++)
		{
			PyObject* item = PyTuple_GetItem(dict, i);
			switch (i)
			{
			case 0:
				(*_value)[0] = ToDoubleVect(item);
				break;

			case 1:
				(*_value)[1] = ToDoubleVect(item);
				(*_value)[2] = ToDoubleVect(item);
				break;

			default:
				break;
			}
		}

		for (auto& item : (*_value)[2])
			item = 0.0;
	}

	void mvShadeSeries::handleSpecificKeywordArgs(PyObject* dict)
	{
		if (dict == nullptr)
			return;

		bool valueChanged = false;
		if (PyObject* item = PyDict_GetItemString(dict, "x")) { valueChanged = true; (*_value)[0] = ToDoubleVect(item); }
		if (PyObject* item = PyDict_GetItemString(dict, "y1")) { valueChanged = true; (*_value)[1] = ToDoubleVect(item); }
		if (PyObject* item = PyDict_GetItemString(dict, "y2")) { valueChanged = true; (*_value)[2] = ToDoubleVect(item); }

		if (valueChanged)
		{
			if ((*_value)[1].size() != (*_value)[2].size())
			{
				(*_value)[2].clear();
				for(size_t i = 0; i < (*_value)[1].size(); i++)
                    (*_value)[2].push_back(0.0);

			}
		}
	}

	void mvShadeSeries::getSpecificConfiguration(PyObject* dict)
	{
		if (dict == nullptr)
			return;
	}

}