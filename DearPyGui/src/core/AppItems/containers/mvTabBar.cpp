#include "mvTabBar.h"
#include "mvApp.h"
#include "mvLog.h"
#include "mvItemRegistry.h"
#include "mvTab.h"
#include "mvPythonExceptions.h"

namespace Marvel {

	void mvTabBar::InsertParser(std::map<std::string, mvPythonParser>* parsers)
	{
		mvPythonParser parser(mvPyDataType::UUID, "Adds a tab bar.", { "Containers", "Widgets" }, true);
		mvAppItem::AddCommonArgs(parser, (CommonParserArgs)(
			MV_PARSER_ARG_ID |
			MV_PARSER_ARG_INDENT |
			MV_PARSER_ARG_PARENT |
			MV_PARSER_ARG_BEFORE |
			MV_PARSER_ARG_CALLBACK |
			MV_PARSER_ARG_SHOW |
			MV_PARSER_ARG_FILTER |
			MV_PARSER_ARG_DROP_CALLBACK |
			MV_PARSER_ARG_DRAG_CALLBACK |
			MV_PARSER_ARG_PAYLOAD_TYPE |
			MV_PARSER_ARG_SEARCH_DELAY |
			MV_PARSER_ARG_TRACKED |
			MV_PARSER_ARG_POS)
		);

		parser.addArg<mvPyDataType::Bool>("reorderable", mvArgType::KEYWORD_ARG, "False", "Allows for the user to change the order of the tabs.");

		parser.finalize();

		parsers->insert({ s_command, parser });
	}

	mvTabBar::mvTabBar(mvUUID uuid)
		:
		mvUUIDPtrBase(uuid)
	{
	}


	bool mvTabBar::canChildBeAdded(mvAppItemType type)
	{
		if (type == mvAppItemType::mvTab)return true;
		if (type == mvAppItemType::mvTabButton)return true;
		if (type == mvAppItemType::mvActivatedHandler) return true;
		if (type == mvAppItemType::mvActiveHandler) return true;
		if (type == mvAppItemType::mvClickedHandler) return true;
		if (type == mvAppItemType::mvDeactivatedAfterEditHandler) return true;
		if (type == mvAppItemType::mvDeactivatedHandler) return true;
		if (type == mvAppItemType::mvEditedHandler) return true;
		if (type == mvAppItemType::mvFocusHandler) return true;
		if (type == mvAppItemType::mvHoverHandler) return true;
		if (type == mvAppItemType::mvResizeHandler) return true;
		if (type == mvAppItemType::mvToggledOpenHandler) return true;
		if (type == mvAppItemType::mvVisibleHandler) return true;

		mvThrowPythonError(mvErrorCode::mvIncompatibleChild, s_command,
			"Incompatible child. Acceptable children include: tab, tab button", this);

		MV_ITEM_REGISTRY_ERROR("TabBar children must be tabs or tab buttons.");
		assert(false);

		return false;
	}

	mvUUID mvTabBar::getSpecificValue()
	{
		return _uiValue;
	}

	void mvTabBar::setValue(mvUUID value)
	{
		_uiValue = value;
	}

	void mvTabBar::draw(ImDrawList* drawlist, float x, float y)
	{
		ScopedID id(_uuid);

		ImGui::BeginGroup();

		if (ImGui::BeginTabBar(_label.c_str(), _flags))
		{

			for (auto& item : _children[1])
			{

				if (!item->preDraw())
					continue;

				if (*_value == item->getUUID() && _lastValue != *_value)
					static_cast<mvTab*>(item.get())->addFlag(ImGuiTabItemFlags_SetSelected);

				item->draw(drawlist, ImGui::GetCursorPosX(), ImGui::GetCursorPosY());

				item->postDraw();

				if (*_value == item->getUUID())
					static_cast<mvTab*>(item.get())->removeFlag(ImGuiTabItemFlags_SetSelected);
			}

			ImGui::EndTabBar();
		}

		ImGui::EndGroup();

		*_value = _uiValue;
		_lastValue = *_value;
	}

	void mvTabBar::handleSpecificKeywordArgs(PyObject* dict)
	{
		if (dict == nullptr)
			return;
		 

		// helper for bit flipping
		auto flagop = [dict](const char* keyword, int flag, int& flags)
		{
			if (PyObject* item = PyDict_GetItemString(dict, keyword)) ToBool(item) ? flags |= flag : flags &= ~flag;
		};

		// window flags
		flagop("reorderable", ImGuiTabBarFlags_Reorderable, _flags);

	}

	void mvTabBar::getSpecificConfiguration(PyObject* dict)
	{
		if (dict == nullptr)
			return;
		 

		// helper to check and set bit
		auto checkbitset = [dict](const char* keyword, int flag, const int& flags)
		{
			PyDict_SetItemString(dict, keyword, ToPyBool(flags & flag));
		};

		// window flags
		checkbitset("reorderable", ImGuiTabBarFlags_Reorderable, _flags);
	}

}