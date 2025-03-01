#pragma once

#include "mvTypeBases.h"
#include "mvApp.h"
#include "mvModule_DearPyGui.h"
#include <string>

namespace Marvel {

    MV_REGISTER_WIDGET(mvSlider3D, MV_ITEM_DESC_DEFAULT, StorageValueTypes::Float4, 1);
    class mvSlider3D : public mvFloat4PtrBase
    {

    public:

        static void InsertParser(std::map<std::string, mvPythonParser>* parsers);

        MV_APPLY_WIDGET_REGISTRATION(mvAppItemType::mvSlider3D, add_3d_slider)

        MV_START_COMMANDS
        MV_END_COMMANDS

        MV_START_CONSTANTS
        MV_END_CONSTANTS

    public:

        mvSlider3D(mvUUID uuid);

        void draw(ImDrawList* drawlist, float x, float y) override;

        void handleSpecificKeywordArgs(PyObject* dict) override;
        void getSpecificConfiguration(PyObject* dict) override;

    private:

        float _minX = 0.0f;
        float _minY = 0.0f;
        float _minZ = 0.0f;
        float _maxX = 100.0f;
        float _maxY = 100.0f;
        float _maxZ = 100.0f;
        float _scale = 1.0f;

    };

}