#pragma once

#include "mvPlot.h"
#include "mvEvents.h"
#include "textures/mvStaticTexture.h"
#include "textures/mvDynamicTexture.h"
#include "textures/mvRawTexture.h"

namespace Marvel {

	MV_REGISTER_WIDGET(mvImageSeries, MV_ITEM_DESC_DEFAULT | MV_ITEM_DESC_CONTAINER, StorageValueTypes::Series, 1);
	class mvImageSeries : public mvSeriesBase
	{
	public:

		static void InsertParser(std::map<std::string, mvPythonParser>* parsers);

		MV_APPLY_WIDGET_REGISTRATION(mvAppItemType::mvImageSeries, add_image_series)

		MV_START_COMMANDS
		MV_END_COMMANDS

		MV_START_CONSTANTS
		MV_END_CONSTANTS

	public:

		mvImageSeries(mvUUID uuid);

		// overriding until we can remove these
		bool preDraw() override { return true; }
		void postDraw() override {}

		void draw(ImDrawList* drawlist, float x, float y) override;
		bool isParentCompatible(mvAppItemType type) override;
		void handleSpecificRequiredArgs(PyObject* args) override;
		void handleSpecificKeywordArgs(PyObject* dict) override;
		void getSpecificConfiguration(PyObject* dict) override;

	private:

		// config
		mvUUID      _textureUUID = 0;
		mvPlotPoint _bounds_min = {0.0, 0.0};
		mvPlotPoint _bounds_max = {0.0, 0.0};
		mvVec2	    _uv_min = { 0.0f, 0.0f };
		mvVec2	    _uv_max = { 1.0f, 1.0f };
		mvColor     _tintColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		
		// pointer to existing item or internal
		std::shared_ptr<mvAppItem> _texture = nullptr;
		bool _internalTexture = false; // create a local texture if necessary

	};

}
