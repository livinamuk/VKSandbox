#pragma once
#include "Callbacks.h"
#include "BackEnd/BackEnd.h"
#include "Editor/Editor.h"
#include "World/SectorManager.h"
#include "World/World.h"
#include <iostream>

namespace Callbacks {
    void NewHeightMap(const std::string& filename) {
        std::cout << "NewHeightMap() callback: " << filename << "\n";
    }

    void NewMap(const std::string& filename) {
        std::cout << "NewMap() callback: " << filename << "\n";
    }

    void NewSector(const std::string& filename) {
        SectorManager::NewSector(filename);
    }

    void OpenHeightMap(const std::string& filename) {
        std::cout << "OpenHeightMap() callback: " << filename << "\n";
    }

    void OpenMap(const std::string& filename) {
        World::LoadMap(filename);
        //Editor::SetCurrentMapName(filename);
    }


    void TestCurrentMap() {
        World::LoadMap("TestMap");
    }

    void OpenSector(const std::string& filename) {
        World::LoadSingleSector(filename);
    }

    void SaveCurrentSector() {
        SectorManager::NewSector(Editor::GetCurrentSectorName());
    }

    void RevertEditorSector() {
        //World::RevertToSavedSector();
        std::cout << "RevertEditorSectorToLastSavedState()\n";
    }

    void OpenHeightMapEditor() {
        if (Editor::GetEditorMode() != EditorMode::HEIGHTMAP_EDITOR) {
            Editor::SetEditorMode(EditorMode::HEIGHTMAP_EDITOR);
            if (Editor::IsEditorClosed()) {
                Editor::OpenEditor();
            }
        }
    }

    void OpenHouseMapEditor() {
        if (Editor::GetEditorMode() != EditorMode::HOUSE_EDITOR) {
            Editor::SetEditorMode(EditorMode::HOUSE_EDITOR);
            if (Editor::IsEditorClosed()) {
                Editor::OpenEditor();
            }
        }
    }
    void OpenMapEditor() {
        Editor::OpenMapEditor();
    }

    void OpenSectorEditor() {
        if (Editor::GetEditorMode() != EditorMode::SECTOR_EDITOR) {
            Editor::SetEditorMode(EditorMode::SECTOR_EDITOR);
            if (Editor::IsEditorClosed()) {
                Editor::OpenEditor();
            }
            World::LoadSingleSector(Editor::GetCurrentSectorName());
        }
    }

    void OpenWeaponsEditor() {
        if (Editor::GetEditorMode() != EditorMode::WEAPON_EDITOR) {
            Editor::SetEditorMode(EditorMode::WEAPON_EDITOR);
            if (Editor::IsEditorClosed()) {
                Editor::OpenEditor();
            }
        }
    }

    void QuitProgram() {
        BackEnd::ForceCloseWindow();
    }
}