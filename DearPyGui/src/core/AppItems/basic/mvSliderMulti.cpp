#include "mvSliderMulti.h"
#include "mvTypeBases.h"
#include <utility>
#include "mvAppItem.h"
#include "mvModule_DearPyGui.h"
#include "mvApp.h"
#include <string>
#include "mvItemRegistry.h"

namespace Marvel {


    void mvSliderFloatMulti::InsertParser(std::map<std::string, mvPythonParser>* parsers)
    {
        mvPythonParser parser(mvPyDataType::UUID, "Adds multi slider for up to 4 float values. Directly entry can be done with double click or CTRL+Click. Min and Max alone are a soft limit for the slider. Use clamped keyword to also apply limits to the direct entry modes.", { "Widgets" });
        mvAppItem::AddCommonArgs(parser, (CommonParserArgs)(
            MV_PARSER_ARG_ID |
            MV_PARSER_ARG_WIDTH |
            MV_PARSER_ARG_INDENT |
            MV_PARSER_ARG_PARENT |
            MV_PARSER_ARG_BEFORE |
            MV_PARSER_ARG_SOURCE |
            MV_PARSER_ARG_CALLBACK |
            MV_PARSER_ARG_SHOW |
            MV_PARSER_ARG_ENABLED |
            MV_PARSER_ARG_FILTER |
            MV_PARSER_ARG_DROP_CALLBACK |
            MV_PARSER_ARG_DRAG_CALLBACK |
            MV_PARSER_ARG_PAYLOAD_TYPE |
            MV_PARSER_ARG_TRACKED |
            MV_PARSER_ARG_POS)
        );

        parser.addArg<mvPyDataType::FloatList>("default_value", mvArgType::KEYWORD_ARG, "(0.0, 0.0, 0.0, 0.0)");

        parser.addArg<mvPyDataType::Integer>("size", mvArgType::KEYWORD_ARG, "4", "Number of components.");

        parser.addArg<mvPyDataType::Bool>("no_input", mvArgType::KEYWORD_ARG, "False", "Disable direct entry methods or Enter key allowing to input text directly into the widget.");
        parser.addArg<mvPyDataType::Bool>("clamped", mvArgType::KEYWORD_ARG, "False", "Applies the min and max limits to direct entry methods also such as double click and CTRL+Click.");

        parser.addArg<mvPyDataType::Float>("min_value", mvArgType::KEYWORD_ARG, "0.0", "Applies a limit only to sliding entry only.");
        parser.addArg<mvPyDataType::Float>("max_value", mvArgType::KEYWORD_ARG, "100.0", "Applies a limit only to sliding entry only.");

        parser.addArg<mvPyDataType::String>("format", mvArgType::KEYWORD_ARG, "'%.3f'");

        parser.finalize();

        parsers->insert({ s_command, parser });
    }

    void mvSliderIntMulti::InsertParser(std::map<std::string, mvPythonParser>* parsers)
    {

        mvPythonParser parser(mvPyDataType::UUID, "Adds multi slider for up to 4 int values. CTRL+Click to directly modify the value.", { "Widgets" });
        mvAppItem::AddCommonArgs(parser, (CommonParserArgs)(
            MV_PARSER_ARG_ID |
            MV_PARSER_ARG_WIDTH |
            MV_PARSER_ARG_INDENT |
            MV_PARSER_ARG_PARENT |
            MV_PARSER_ARG_BEFORE |
            MV_PARSER_ARG_SOURCE |
            MV_PARSER_ARG_CALLBACK |
            MV_PARSER_ARG_SHOW |
            MV_PARSER_ARG_ENABLED |
            MV_PARSER_ARG_FILTER |
            MV_PARSER_ARG_DROP_CALLBACK |
            MV_PARSER_ARG_DRAG_CALLBACK |
            MV_PARSER_ARG_PAYLOAD_TYPE |
            MV_PARSER_ARG_TRACKED |
            MV_PARSER_ARG_POS)
        );

        parser.addArg<mvPyDataType::IntList>("default_value", mvArgType::KEYWORD_ARG, "(0, 0, 0, 0)");

        parser.addArg<mvPyDataType::Integer>("size", mvArgType::KEYWORD_ARG, "4", "number of components");

        parser.addArg<mvPyDataType::Bool>("no_input", mvArgType::KEYWORD_ARG, "False", "Disable direct entry methods or Enter key allowing to input text directly into the widget.");
        parser.addArg<mvPyDataType::Bool>("clamped", mvArgType::KEYWORD_ARG, "False", "Applies the min and max limits to direct entry methods also such as double click and CTRL+Click.");

        parser.addArg<mvPyDataType::Integer>("min_value", mvArgType::KEYWORD_ARG, "0", "Applies a limit only to sliding entry only.");
        parser.addArg<mvPyDataType::Integer>("max_value", mvArgType::KEYWORD_ARG, "100", "Applies a limit only to sliding entry only.");

        parser.addArg<mvPyDataType::String>("format", mvArgType::KEYWORD_ARG, "'%d'");

        parser.finalize();

        parsers->insert({ s_command, parser });

    }

    mvSliderFloatMulti::mvSliderFloatMulti(mvUUID uuid)
        : mvFloat4PtrBase(uuid)
    {
    }

    void mvSliderFloatMulti::setEnabled(bool value)
    {
        if (value == _enabled)
            return;

        if (value)
            _flags = _stor_flags;

        else
        {
            _stor_flags = _flags;
            _flags |= ImGuiSliderFlags_NoInput;
        }

        _enabled = value;
    }

    void mvSliderFloatMulti::draw(ImDrawList* drawlist, float x, float y)
    {
        ScopedID id(_uuid);

        if (!_enabled) std::copy(_value->data(), _value->data() + 4, _disabled_value);

        switch (_size)
        {
        case 2:
            if (ImGui::SliderFloat2(_label.c_str(), _enabled ? _value->data() : &_disabled_value[0], _min, _max, _format.c_str(), _flags))
            {
                auto value = *_value;
                mvApp::GetApp()->getCallbackRegistry().submitCallback([=]() {
                    mvApp::GetApp()->getCallbackRegistry().addCallback(getCallback(false), _uuid, ToPyFloatList(value.data(), value.size()), _user_data);
                    });
            }
            break;
        case 3:
            if (ImGui::SliderFloat3(_label.c_str(), _enabled ? _value->data() : &_disabled_value[0], _min, _max, _format.c_str(), _flags))
            {
                auto value = *_value;
                mvApp::GetApp()->getCallbackRegistry().submitCallback([=]() {
                    mvApp::GetApp()->getCallbackRegistry().addCallback(getCallback(false), _uuid, ToPyFloatList(value.data(), value.size()), _user_data);
                    });
            }
            break;
        case 4:
            if (ImGui::SliderFloat4(_label.c_str(), _enabled ? _value->data() : &_disabled_value[0], _min, _max, _format.c_str(), _flags))
            {
                auto value = *_value;
                mvApp::GetApp()->getCallbackRegistry().submitCallback([=]() {
                    mvApp::GetApp()->getCallbackRegistry().addCallback(getCallback(false), _uuid, ToPyFloatList(value.data(), value.size()), _user_data);
                    });
            }
            break;
        default:
            break;
        }
    }

    mvSliderIntMulti::mvSliderIntMulti(mvUUID uuid)
        : mvInt4PtrBase(uuid)
    {
    }

    void mvSliderIntMulti::setEnabled(bool value)
    {
        if (value == _enabled)
            return;

        if (value)
            _flags = _stor_flags;

        else
        {
            _stor_flags = _flags;
            _flags |= ImGuiSliderFlags_NoInput;
        }

        _enabled = value;
    }

    void mvSliderIntMulti::draw(ImDrawList* drawlist, float x, float y)
    {
        ScopedID id(_uuid);

        if (!_enabled) std::copy(_value->data(), _value->data() + 4, _disabled_value);

        switch (_size)
        {
        case 2:
            if (ImGui::SliderInt2(_label.c_str(), _enabled ? _value->data() : &_disabled_value[0], _min, _max, _format.c_str(), _flags))
            {
                auto value = *_value;
                mvApp::GetApp()->getCallbackRegistry().submitCallback([=]() {
                    mvApp::GetApp()->getCallbackRegistry().addCallback(getCallback(false), _uuid, ToPyIntList(value.data(), value.size()), _user_data);
                    });
            }
            break;
        case 3:
            if (ImGui::SliderInt3(_label.c_str(), _enabled ? _value->data() : &_disabled_value[0], _min, _max, _format.c_str(), _flags))
            {
                auto value = *_value;
                mvApp::GetApp()->getCallbackRegistry().submitCallback([=]() {
                    mvApp::GetApp()->getCallbackRegistry().addCallback(getCallback(false), _uuid, ToPyIntList(value.data(), value.size()), _user_data);
                    });
            }
            break;
        case 4:
            if (ImGui::SliderInt4(_label.c_str(), _enabled ? _value->data() : &_disabled_value[0], _min, _max, _format.c_str(), _flags))
            {
                auto value = *_value;
                mvApp::GetApp()->getCallbackRegistry().submitCallback([=]() {
                    mvApp::GetApp()->getCallbackRegistry().addCallback(getCallback(false), _uuid, ToPyIntList(value.data(), value.size()), _user_data);
                    });
            }
            break;
        default:
            break;
        }

    }

    void mvSliderFloatMulti::handleSpecificKeywordArgs(PyObject* dict)
    {
        if (dict == nullptr)
            return;

        if (PyObject* item = PyDict_GetItemString(dict, "format")) _format = ToString(item);
        if (PyObject* item = PyDict_GetItemString(dict, "min_value")) _min = ToFloat(item);
        if (PyObject* item = PyDict_GetItemString(dict, "max_value")) _max = ToFloat(item);
        if (PyObject* item = PyDict_GetItemString(dict, "size")) _size = ToInt(item);

        // helper for bit flipping
        auto flagop = [dict](const char* keyword, int flag, int& flags)
        {
            if (PyObject* item = PyDict_GetItemString(dict, keyword)) ToBool(item) ? flags |= flag : flags &= ~flag;
        };

        // flags
        flagop("clamped", ImGuiSliderFlags_ClampOnInput, _flags);
        flagop("clamped", ImGuiSliderFlags_ClampOnInput, _stor_flags);
        flagop("no_input", ImGuiSliderFlags_NoInput, _flags);
        flagop("no_input", ImGuiSliderFlags_NoInput, _stor_flags);

    }

    void mvSliderFloatMulti::getSpecificConfiguration(PyObject* dict)
    {
        if (dict == nullptr)
            return;

        PyDict_SetItemString(dict, "format", ToPyString(_format));
        PyDict_SetItemString(dict, "min_value", ToPyFloat(_min));
        PyDict_SetItemString(dict, "max_value", ToPyFloat(_max));
        PyDict_SetItemString(dict, "size", ToPyInt(_size));

        // helper to check and set bit
        auto checkbitset = [dict](const char* keyword, int flag, const int& flags)
        {
            PyDict_SetItemString(dict, keyword, ToPyBool(flags & flag));
        };

        // window flags
        checkbitset("clamped", ImGuiSliderFlags_ClampOnInput, _flags);
        checkbitset("no_input", ImGuiSliderFlags_NoInput, _flags);

    }

    void mvSliderIntMulti::handleSpecificKeywordArgs(PyObject* dict)
    {
        if (dict == nullptr)
            return;
         
        if (PyObject* item = PyDict_GetItemString(dict, "format")) _format = ToString(item);
        if (PyObject* item = PyDict_GetItemString(dict, "min_value")) _min = ToInt(item);
        if (PyObject* item = PyDict_GetItemString(dict, "max_value")) _max = ToInt(item);
        if (PyObject* item = PyDict_GetItemString(dict, "size")) _size = ToInt(item);

        // helper for bit flipping
        auto flagop = [dict](const char* keyword, int flag, int& flags)
        {
            if (PyObject* item = PyDict_GetItemString(dict, keyword)) ToBool(item) ? flags |= flag : flags &= ~flag;
        };

        // flags
        flagop("clamped", ImGuiSliderFlags_ClampOnInput, _flags);
        flagop("clamped", ImGuiSliderFlags_ClampOnInput, _stor_flags);
        flagop("no_input", ImGuiSliderFlags_NoInput, _flags);
        flagop("no_input", ImGuiSliderFlags_NoInput, _stor_flags);

    }

    void mvSliderIntMulti::getSpecificConfiguration(PyObject* dict)
    {
        if (dict == nullptr)
            return;
         
        PyDict_SetItemString(dict, "format", ToPyString(_format));
        PyDict_SetItemString(dict, "min_value", ToPyInt(_min));
        PyDict_SetItemString(dict, "max_value", ToPyInt(_max));
        PyDict_SetItemString(dict, "size", ToPyInt(_size));

        // helper to check and set bit
        auto checkbitset = [dict](const char* keyword, int flag, const int& flags)
        {
            PyDict_SetItemString(dict, keyword, ToPyBool(flags & flag));
        };

        // window flags
        checkbitset("clamped", ImGuiSliderFlags_ClampOnInput, _flags);
        checkbitset("no_input", ImGuiSliderFlags_NoInput, _flags);

    }

}
