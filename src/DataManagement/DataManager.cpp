//
// Created by katie on 23/06/2026.
//

#include <Geode/Geode.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include "DataManager.hpp"

using namespace AutoPauseMod::DataManagement;

void DataManager::UpdateLevelInformation() {
    PlayLayer* playLayer = PlayLayer::get();
    auto* level = playLayer? playLayer->m_level: nullptr;

    //ignore unsupported game states.
    if (!level || level->isPlatformer() || LevelEditorLayer::get()) {
        this->m_bIgnoreState = true;
        return;
    }

    DataPersistence::SerialiseAndSaveWaypoints(this->m_loadedLevelWaypoints);
    this->m_loadedLevelWaypoints.clear();
}
