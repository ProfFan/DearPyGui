#pragma once

#include <utility>
#include <array>
#include "mvAppItem.h"
#include "mvItemRegistry.h"
#include "textures/mvStaticTexture.h"
#include "textures/mvDynamicTexture.h"
#include "textures/mvRawTexture.h"

namespace Marvel {

	MV_REGISTER_WIDGET(mvRawCanvas, MV_ITEM_DESC_DEFAULT, StorageValueTypes::None, 1);
	class mvRawCanvas : public mvAppItem
	{

	public:

		static void InsertParser(std::map<std::string, mvPythonParser>* parsers);

		MV_APPLY_WIDGET_REGISTRATION(mvAppItemType::mvRawCanvas, add_raw_canvas)

		MV_START_COMMANDS
		MV_END_COMMANDS

		MV_START_CONSTANTS
		MV_END_CONSTANTS

		explicit mvRawCanvas(mvUUID uuid);

		void draw(ImDrawList* drawlist, float x, float y) override;

		void handleSpecificRequiredArgs(PyObject* dict) override;
		void handleSpecificKeywordArgs(PyObject* dict) override;
		void getSpecificConfiguration(PyObject* dict) override;

		void   setValue          (mvUUID value);
		mvUUID get1Value          () const;

	private:

		// config
		void*  _textureHandle = 0;
		mvVec2	_uv_min = {0.0f, 0.0f};
		mvVec2	_uv_max = {1.0f, 1.0f};
		mvColor _tintColor = {1.0f, 1.0f, 1.0f, 1.0f};
		mvColor _borderColor = {0.0f, 0.0f, 0.0f, 0.0f};

		bool _internalTexture = false; // create a local texture if necessary

	};

}