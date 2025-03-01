#pragma once

#include "mvTypeBases.h"

namespace Marvel {

	MV_REGISTER_WIDGET(mvPlotAxis, MV_ITEM_DESC_DEFAULT | MV_ITEM_DESC_CONTAINER, StorageValueTypes::None, 1);
	class mvPlotAxis : public mvAppItem
	{

	public:

		static void InsertParser(std::map<std::string, mvPythonParser>* parsers);

		MV_APPLY_WIDGET_REGISTRATION(mvAppItemType::mvPlotAxis, add_plot_axis)

		MV_CREATE_CONSTANT(mvXAxis, 0);
		MV_CREATE_CONSTANT(mvYAxis, 1);

		MV_CREATE_COMMAND(reset_axis_ticks);
		MV_CREATE_COMMAND(set_axis_ticks);
		MV_CREATE_COMMAND(set_axis_limits);
		MV_CREATE_COMMAND(set_axis_limits_auto);
		MV_CREATE_COMMAND(get_axis_limits);
		MV_CREATE_COMMAND(fit_axis_data);

		MV_START_COMMANDS
			MV_ADD_COMMAND(reset_axis_ticks);
			MV_ADD_COMMAND(set_axis_ticks);
			MV_ADD_COMMAND(set_axis_limits);
			MV_ADD_COMMAND(set_axis_limits_auto);
			MV_ADD_COMMAND(get_axis_limits);
			MV_ADD_COMMAND(fit_axis_data);
		MV_END_COMMANDS

		MV_START_CONSTANTS
			MV_ADD_CONSTANT(mvXAxis),
			MV_ADD_CONSTANT(mvYAxis)
		MV_END_CONSTANTS

	public:

		mvPlotAxis(mvUUID uuid);

		void draw(ImDrawList* drawlist, float x, float y) override;
		void customAction() override;
		bool isParentCompatible(mvAppItemType type) override;
		bool canChildBeAdded(mvAppItemType type) override;
		void onChildRemoved(mvRef<mvAppItem> item) override;
		void onChildAdd(mvRef<mvAppItem> item) override;
		void handleSpecificKeywordArgs(PyObject* dict) override;
		void handleSpecificRequiredArgs(PyObject* args) override;
		void getSpecificConfiguration(PyObject* dict) override;
		void hide() override;
		void show() override;
		void setYTicks(const std::vector<std::string>& labels, const std::vector<double>& locations);
		void resetYTicks();
		void postDraw() override;
		void fitAxisData();

		void setLimits(float y_min, float y_max);
		void setLimitsAuto();

		ImPlotAxisFlags getFlags() const { return _flags; }
		const ImVec2& getYLimits() const { return _limits_actual; }

	private:

		ImPlotAxisFlags          _flags = 0;
		int                      _axis = 0;
		bool                     _setLimits = false;
		ImVec2                   _limits;
		ImVec2                   _limits_actual;
		std::vector<std::string> _labels;
		std::vector<double>      _labelLocations;
		std::vector<const char*> _clabels; // to prevent conversion from string to char* every frame
		bool                     _dirty = false;

	};

}
