#include "mvSliderSingle.h"
#include "mvTypeBases.h"
#include <utility>
#include "mvAppItem.h"
#include "mvModule_DearPyGui.h"
#include "mvApp.h"
#include <string>
#include "mvItemRegistry.h"

namespace Marvel {

    void mvSliderFloat::InsertParser(std::map<std::string, mvPythonParser>* parsers)
    {
        mvPythonParser parser(mvPyDataType::UUID, "Adds slider for a single float value. Directly entry can be done with double click or CTRL+Click. Min and Max alone are a soft limit for the slider. Use clamped keyword to also apply limits to the direct entry modes.", { "Widgets" });
        mvAppItem::AddCommonArgs(parser, (CommonParserArgs)(
            MV_PARSER_ARG_ID |
            MV_PARSER_ARG_WIDTH |
            MV_PARSER_ARG_HEIGHT |
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

        parser.addArg<mvPyDataType::Float>("default_value", mvArgType::KEYWORD_ARG, "0.0");

        parser.addArg<mvPyDataType::Bool>("vertical", mvArgType::KEYWORD_ARG, "False", "Sets orientation to vertical.");
        parser.addArg<mvPyDataType::Bool>("no_input", mvArgType::KEYWORD_ARG, "False", "Disable direct entry methods or Enter key allowing to input text directly into the widget.");
        parser.addArg<mvPyDataType::Bool>("clamped", mvArgType::KEYWORD_ARG, "False", "Applies the min and max limits to direct entry methods also such as double click and CTRL+Click.");

        parser.addArg<mvPyDataType::Float>("min_value", mvArgType::KEYWORD_ARG, "0.0", "Applies a limit only to sliding entry only.");
        parser.addArg<mvPyDataType::Float>("max_value", mvArgType::KEYWORD_ARG, "100.0", "Applies a limit only to sliding entry only.");

        parser.addArg<mvPyDataType::String>("format", mvArgType::KEYWORD_ARG, "'%.3f'");

        parser.finalize();

        parsers->insert({ s_command, parser });

    }

    void mvSliderInt::InsertParser(std::map<std::string, mvPythonParser>* parsers)
    {
        mvPythonParser parser(mvPyDataType::UUID, "Adds slider for a single int value. Directly entry can be done with double click or CTRL+Click. Min and Max alone are a soft limit for the slider. Use clamped keyword to also apply limits to the direct entry modes.", { "Widgets" });
        mvAppItem::AddCommonArgs(parser, (CommonParserArgs)(
            MV_PARSER_ARG_ID |
            MV_PARSER_ARG_WIDTH |
            MV_PARSER_ARG_HEIGHT |
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

        parser.addArg<mvPyDataType::Integer>("default_value", mvArgType::KEYWORD_ARG, "0");

        parser.addArg<mvPyDataType::Bool>("vertical", mvArgType::KEYWORD_ARG, "False", "Sets orientation to vertical.");
        parser.addArg<mvPyDataType::Bool>("no_input", mvArgType::KEYWORD_ARG, "False", "Disable direct entry methods or Enter key allowing to input text directly into the widget.");
        parser.addArg<mvPyDataType::Bool>("clamped", mvArgType::KEYWORD_ARG, "False", "Applies the min and max limits to direct entry methods also such as double click and CTRL+Click.");

        parser.addArg<mvPyDataType::Integer>("min_value", mvArgType::KEYWORD_ARG, "0", "Applies a limit only to sliding entry only.");
        parser.addArg<mvPyDataType::Integer>("max_value", mvArgType::KEYWORD_ARG, "100", "Applies a limit only to sliding entry only.");

        parser.addArg<mvPyDataType::String>("format", mvArgType::KEYWORD_ARG, "'%d'");

        parser.finalize();

        parsers->insert({ s_command, parser });
    }

    mvSliderFloat::mvSliderFloat(mvUUID uuid)
        : mvFloatPtrBase(uuid)
    {
    }

    void mvSliderFloat::setEnabled(bool value)
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

    void mvSliderFloat::draw(ImDrawList* drawlist, float x, float y)
    {
        ScopedID id(_uuid);
        ////mvImGuiThemeScope scope(this);
        //mvFontScope fscope(this);

        if (!_enabled) _disabled_value = *_value;

        if (_vertical)
        {
            if ((float)_height < 1.0f)
                _height = 100;
            if ((float)_width < 1.0f)
                _width = 20;

            if (ImGui::VSliderFloat(_label.c_str(), ImVec2((float)_width, (float)_height), _enabled ? _value.get() : &_disabled_value, _min, _max, _format.c_str()))
            {
                auto value = *_value;
                mvApp::GetApp()->getCallbackRegistry().submitCallback([=]() {
                    mvApp::GetApp()->getCallbackRegistry().addCallback(getCallback(false), _uuid, ToPyFloat(value), _user_data);
                    });
            }

        }
        else
        {
            if (ImGui::SliderFloat(_label.c_str(), _enabled ? _value.get() : &_disabled_value, _min, _max, _format.c_str(), _flags))
            {
                auto value = *_value;
                mvApp::GetApp()->getCallbackRegistry().submitCallback([=]() {
                    mvApp::GetApp()->getCallbackRegistry().addCallback(getCallback(false), _uuid, ToPyFloat(value), _user_data);
                    });
            }

        }
    }

    mvSliderInt::mvSliderInt(mvUUID uuid)
        : mvIntPtrBase(uuid)
    {
    }

    void mvSliderInt::setEnabled(bool value)
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

    void mvSliderInt::draw(ImDrawList* drawlist, float x, float y)
    {
        ScopedID id(_uuid);

        if (!_enabled) _disabled_value = *_value;

        if (_vertical)
        {
            if ((float)_height < 1.0f)
                _height = 100;
            if ((float)_width < 1.0f)
                _width = 20;

            if (ImGui::VSliderInt(_label.c_str(), ImVec2((float)_width, (float)_height), _enabled ? _value.get() : &_disabled_value, _min, _max, _format.c_str()))
            {
                auto value = *_value;
                mvApp::GetApp()->getCallbackRegistry().submitCallback([=]() {
                    mvApp::GetApp()->getCallbackRegistry().addCallback(getCallback(false), _uuid, ToPyInt(value), _user_data);
                    });
            }
        }
        else
        {
            if (ImGui::SliderInt(_label.c_str(), _enabled ? _value.get() : &_disabled_value, _min, _max, _format.c_str(), _flags))
            {
                auto value = *_value;
                mvApp::GetApp()->getCallbackRegistry().submitCallback([=]() {
                    mvApp::GetApp()->getCallbackRegistry().addCallback(getCallback(false), _uuid, ToPyInt(value), _user_data);
                    });
            }

        }
    }

    void mvSliderFloat::handleSpecificKeywordArgs(PyObject* dict)
    {
        if (dict == nullptr)
            return;

        if (PyObject* item = PyDict_GetItemString(dict, "format")) _format = ToString(item);
        if (PyObject* item = PyDict_GetItemString(dict, "vertical")) _vertical = ToBool(item);
        if (PyObject* item = PyDict_GetItemString(dict, "min_value")) _min = ToFloat(item);
        if (PyObject* item = PyDict_GetItemString(dict, "max_value")) _max = ToFloat(item);

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

    void mvSliderFloat::getSpecificConfiguration(PyObject* dict)
    {
        if (dict == nullptr)
            return;

        PyDict_SetItemString(dict, "format", ToPyString(_format));
        PyDict_SetItemString(dict, "vertical", ToPyBool(_vertical));
        PyDict_SetItemString(dict, "min_value", ToPyFloat(_min));
        PyDict_SetItemString(dict, "max_value", ToPyFloat(_max));

        // helper to check and set bit
        auto checkbitset = [dict](const char* keyword, int flag, const int& flags)
        {
            PyDict_SetItemString(dict, keyword, ToPyBool(flags & flag));
        };

        // window flags
        checkbitset("clamped", ImGuiSliderFlags_ClampOnInput, _flags);
        checkbitset("no_input", ImGuiSliderFlags_NoInput, _flags);

    }

    void mvSliderInt::handleSpecificKeywordArgs(PyObject* dict)
    {
        if (dict == nullptr)
            return;

        if (PyObject* item = PyDict_GetItemString(dict, "format")) _format = ToString(item);
        if (PyObject* item = PyDict_GetItemString(dict, "vertical")) _vertical = ToBool(item);
        if (PyObject* item = PyDict_GetItemString(dict, "min_value")) _min = ToInt(item);
        if (PyObject* item = PyDict_GetItemString(dict, "max_value")) _max = ToInt(item);

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

    void mvSliderInt::getSpecificConfiguration(PyObject* dict)
    {
        if (dict == nullptr)
            return;

        PyDict_SetItemString(dict, "format", ToPyString(_format));
        PyDict_SetItemString(dict, "vertical", ToPyBool(_vertical));
        PyDict_SetItemString(dict, "min_value", ToPyInt(_min));
        PyDict_SetItemString(dict, "max_value", ToPyInt(_max));

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
