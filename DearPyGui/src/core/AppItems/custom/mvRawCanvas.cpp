#include "mvRawCanvas.h"
#include "mvAppItemState.h"
#include "mvItemRegistry.h"
#include "mvApp.h"
#include "mvPythonExceptions.h"
#include "tupleobject.h"

namespace Marvel {

	void mvRawCanvas::InsertParser(std::map<std::string, mvPythonParser>* parsers)
	{

		mvPythonParser parser(mvPyDataType::UUID, "Adds an image from a specified texture. uv_min and uv_max represent the normalized texture coordinates of the original image that will be shown. Using range (0.0,0.0)->(1.0,1.0) for texture coordinates will generally display the entire texture.", { "Textures", "Widgets" });
		mvAppItem::AddCommonArgs(parser, (CommonParserArgs)(
			MV_PARSER_ARG_ID |
			MV_PARSER_ARG_WIDTH |
			MV_PARSER_ARG_HEIGHT |
			MV_PARSER_ARG_INDENT |
			MV_PARSER_ARG_PARENT |
			MV_PARSER_ARG_BEFORE |
			MV_PARSER_ARG_CALLBACK |
			MV_PARSER_ARG_SOURCE |
			MV_PARSER_ARG_SHOW |
			MV_PARSER_ARG_FILTER |
			MV_PARSER_ARG_DROP_CALLBACK |
			MV_PARSER_ARG_DRAG_CALLBACK |
			MV_PARSER_ARG_PAYLOAD_TYPE |
			MV_PARSER_ARG_TRACKED |
			MV_PARSER_ARG_POS)
		);

		parser.addArg<mvPyDataType::UUID>("texture_handle");
		
		parser.addArg<mvPyDataType::FloatList>("tint_color", mvArgType::KEYWORD_ARG, "(255, 255, 255, 255)", "Applies a color tint to the entire texture.");
		parser.addArg<mvPyDataType::FloatList>("border_color", mvArgType::KEYWORD_ARG, "(0, 0, 0, 0)", "Displays a border of the specified color around the texture.");
		parser.addArg<mvPyDataType::FloatList>("uv_min", mvArgType::KEYWORD_ARG, "(0.0, 0.0)", "Normalized texture coordinates min point.");
		parser.addArg<mvPyDataType::FloatList>("uv_max", mvArgType::KEYWORD_ARG, "(1.0, 1.0)", "Normalized texture coordinates max point.");

		parser.finalize();

		parsers->insert({ s_command, parser });
	}

	mvRawCanvas::mvRawCanvas(mvUUID uuid)
		: mvAppItem(uuid)
	{
		_width = 0;
		_height = 0;
	}


	void mvRawCanvas::draw(ImDrawList* drawlist, float x, float y)
	{

		if (_textureHandle)
		{
			ImVec2 avail_size = ImGui::GetContentRegionAvail();

			mvApp::GetApp()->getCallbackRegistry().submitCallback([=]() {
				PyObject* app_data = PyTuple_New(2);
				PyObject* size_tuple = PyTuple_New(2);

				PyTuple_SetItem(size_tuple, 0, ToPyInt(avail_size.x));
				PyTuple_SetItem(size_tuple, 1, ToPyInt(avail_size.y));
				PyTuple_SetItem(app_data, 0, ToPyUUID(0));
				PyTuple_SetItem(app_data, 1, size_tuple);
				mvApp::GetApp()->getCallbackRegistry().addCallback(getCallback(false), _uuid, app_data, _user_data);
			});

			ScopedID id(_uuid);

			ImGui::Image(_textureHandle, ImVec2(_width, _height), ImVec2(_uv_min.x, _uv_min.y), ImVec2(_uv_max.x, _uv_max.y),
				ImVec4((float)_tintColor.r, (float)_tintColor.g, (float)_tintColor.b, (float)_tintColor.a),
				ImVec4((float)_borderColor.r, (float)_borderColor.g, (float)_borderColor.b, (float)_borderColor.a));

			ImGuiIO& io = ImGui::GetIO();

			bool value_changed = false;
			ImGui::InvisibleButton(_specificedlabel.c_str(), avail_size);
    		bool is_active = ImGui::IsItemActive();
			if (is_active && (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f)) {
				value_changed = true;
			}

			if (value_changed) {
				mvApp::GetApp()->getCallbackRegistry().submitCallback([=]() {
					PyObject* app_data = PyTuple_New(2);
					PyObject* delta_tuple = PyTuple_New(2);

					PyTuple_SetItem(delta_tuple, 0, ToPyInt(io.MouseDelta.x));
					PyTuple_SetItem(delta_tuple, 1, ToPyInt(io.MouseDelta.y));
					PyTuple_SetItem(app_data, 0, ToPyUUID(1));
					PyTuple_SetItem(app_data, 1, delta_tuple);
					mvApp::GetApp()->getCallbackRegistry().addCallback(getCallback(false), _uuid, app_data, _user_data);
				});
			}
		}

	}

	void mvRawCanvas::setValue(mvUUID value)
	{ 
		_textureHandle = (void*)value;
	}

	mvUUID mvRawCanvas::get1Value() const
	{ 
		return (mvUUID)_textureHandle;
	}

	void mvRawCanvas::handleSpecificRequiredArgs(PyObject* dict)
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
				_textureHandle = (void*)ToUUID(item);
				if (_textureHandle)
					break;
				else
				{
					mvThrowPythonError(mvErrorCode::mvTextureNotFound, s_command, "Texture not found.", this);
					break;
				}
			}

			default:
				break;
			}
		}
	}

	void mvRawCanvas::handleSpecificKeywordArgs(PyObject* dict)
	{
		if (dict == nullptr)
			return;

		if (PyObject* item = PyDict_GetItemString(dict, "uv_min")) _uv_min = ToVec2(item);
		if (PyObject* item = PyDict_GetItemString(dict, "uv_max")) _uv_max = ToVec2(item);
		if (PyObject* item = PyDict_GetItemString(dict, "tint_color")) _tintColor = ToColor(item);
		if (PyObject* item = PyDict_GetItemString(dict, "border_color")) _borderColor = ToColor(item);
	}

	void mvRawCanvas::getSpecificConfiguration(PyObject* dict)
	{
		if (dict == nullptr)
			return;

		PyDict_SetItemString(dict, "uv_min", ToPyPair(_uv_min.x, _uv_min.y));
		PyDict_SetItemString(dict, "uv_max", ToPyPair(_uv_max.x, _uv_max.y));
		PyDict_SetItemString(dict, "tint_color", ToPyColor(_tintColor));
		PyDict_SetItemString(dict, "border_color", ToPyColor(_borderColor));
		PyDict_SetItemString(dict, "texture_handle", ToPyUUID((mvUUID)_textureHandle));
	}

}