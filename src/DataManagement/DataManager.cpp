//
// Created by katie on 23/06/2026.
//

#include <Geode/Geode.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include "DataManager.hpp"

using namespace AutoPauseMod::DataManagement;
using namespace AutoPauseMod::Waypoints;
using namespace geode::prelude;

void DataManager::UpdateLevelInformation() {
    PlayLayer* playLayer = PlayLayer::get();
    auto* level = playLayer? playLayer->m_level: nullptr;

    DataPersistence::SerialiseAndSaveWaypoints(this->m_loadedLevelWaypoints);
    this->m_loadedLevelWaypoints.clear();

    //ignore unsupported game states.
    if (!level || level->isPlatformer() || LevelEditorLayer::get()) {
        this->m_bIgnoreState = true;
        return;
    } else this->m_bIgnoreState = false;

    //for editor levels, we'll use the level ID api
    this->m_currentLevelID = level->m_isUploaded? level->m_levelID.value(): EditorIDs::getID(level);

    //...and load those waypoints.
    this->m_loadedLevelWaypoints = DataPersistence::LoadLevelWaypoints(this->m_currentLevelID);

    std::ranges::sort(this->m_loadedLevelWaypoints,
    [](const std::shared_ptr<Waypoint>& a, const std::shared_ptr<Waypoint>& b) -> bool {
            return a->GetTriggerPercentage() < b->GetTriggerPercentage();
        }
    );

    //TODO: update UI when i actually made it sob
}



std::shared_ptr<Waypoint> DataManager::NewWaypoint() {
    //PlayLayer's always gonna exist so long as the UI is open.
    float percentage = PlayLayer::get()->getCurrentPercent();

    auto waypoint = std::make_shared<Waypoint>(
        WaypointBehaviourType::FromStartOnly,
        percentage,
        this->m_currentLevelID
    );

    auto pos = std::ranges::lower_bound(
        this->m_loadedLevelWaypoints,
        waypoint->GetTriggerPercentage(),
        std::ranges::less{},

        [](const std::shared_ptr<Waypoint>& other) -> float {
            return other->GetTriggerPercentage();
        }
    );

    this->m_loadedLevelWaypoints.insert(pos, waypoint);
    return waypoint;
}

void DataManager::ToggleWaypoint(const std::shared_ptr<Waypoint>& waypoint) {
    if (!waypoint->IsGlobal()) {
        const auto it = std::ranges::find(this->m_loadedLevelWaypoints, waypoint);
        if (it == this->m_loadedLevelWaypoints.end()) return;

        this->m_loadedLevelWaypoints.erase(it);

        waypoint->SetGlobal(true);

        const auto pos = std::ranges::lower_bound(
            this->m_loadedGlobalWaypoints,
            waypoint->GetTriggerPercentage(),
            std::ranges::less{},

            [](const std::shared_ptr<Waypoint>& other) -> float {
                return other->GetTriggerPercentage();
            }
        );

        this->m_loadedGlobalWaypoints.insert(pos, waypoint);

        //we have a new global waypoint, save data
        DataPersistence::SerialiseAndSaveWaypoints(this->m_loadedGlobalWaypoints);
    }

    else {
        const auto it = std::ranges::find(this->m_loadedGlobalWaypoints, waypoint);
        if (it == this->m_loadedGlobalWaypoints.end()) return;

        this->m_loadedGlobalWaypoints.erase(it);

        waypoint->SetGlobal(false);
        waypoint->SetLevelID(this->m_currentLevelID);

        const auto pos = std::ranges::lower_bound(
            this->m_loadedLevelWaypoints,
            waypoint->GetTriggerPercentage(),
            std::ranges::less{},

            [](const std::shared_ptr<Waypoint>& other) -> float {
                return other->GetTriggerPercentage();
            }
        );

        this->m_loadedLevelWaypoints.insert(pos, waypoint);
    }
}

void DataManager::DeleteWaypoint(const std::shared_ptr<Waypoint>& waypoint) {
    if (waypoint->IsGlobal()) {
        auto it = std::ranges::find(this->m_loadedGlobalWaypoints, waypoint);
        if (it != this->m_loadedGlobalWaypoints.end()) this->m_loadedGlobalWaypoints.erase(it);

        //global waypoints changed, save em
        DataPersistence::SerialiseAndSaveWaypoints(this->m_loadedGlobalWaypoints);
    }

    else {
        auto it = std::ranges::find(this->m_loadedLevelWaypoints, waypoint);
        if (it != this->m_loadedLevelWaypoints.end()) this->m_loadedLevelWaypoints.erase(it);
    }
}

void DataManager::SetShouldPauseOnNewBest(bool newState) {
    this->m_bPauseOnNewBest = newState;
    Mod::get()->setSavedValue<bool>("pauseOnNewBest", newState);
}

void DataManager::SetAttemptStartPercentage(float percentage) {
    this->m_attemptStartPercent = percentage;
}

void DataManager::SetShouldIgnorePracticeMode(bool newState) {
    this->m_bIgnorePracticeMode = newState;
    Mod::get()->setSavedValue<bool>("ignorePracticeMode", newState);
}

bool DataManager::CheckWaypoints(const float currentPercentage) const {
    static const auto shouldPause = [&](const std::shared_ptr<Waypoint>& waypoint) -> bool {
        return waypoint->ShouldPause(this, currentPercentage);
    };

    return std::ranges::any_of(this->m_loadedGlobalWaypoints, shouldPause) ||
        std::ranges::any_of(this->m_loadedLevelWaypoints, shouldPause);
}

//gonna do some textbook RAII bc uhh its cool :3
//okay but realistically its actually useful
DataManager::DataManager() {
    this->m_loadedGlobalWaypoints = DataPersistence::LoadGlobalWaypoints();
    this->m_bPauseOnNewBest = Mod::get()->getSavedValue<bool>("pauseOnNewBest", true);
    this->m_bIgnorePracticeMode = Mod::get()->getSavedValue<bool>("ignorePracticeMode", true);
}