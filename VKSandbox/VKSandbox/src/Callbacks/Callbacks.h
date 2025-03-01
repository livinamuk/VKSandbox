#pragma once
#include "HellFunctionTypes.h"

namespace Callbacks {
    void NewHeightMap(const std::string& filename);
    void NewMap(const std::string& filename);
    void NewSector(const std::string& filename);

    void OpenMap(const std::string& filename);
    void OpenHeightMap(const std::string& filename);
    void OpenSector(const std::string& filename);
    void TestCurrentMap();

    void OpenHouseMapEditor();
    void OpenHeightMapEditor();
    void OpenMapEditor();
    void OpenSectorEditor();
    void OpenWeaponsEditor();

    void SaveCurrentSector();
    void RevertEditorSector();

    void QuitProgram();
}