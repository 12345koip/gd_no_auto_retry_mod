//
// Created by katie on 23/06/2026.
//

#include <Geode/Geode.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include "DataManager.hpp"
#include "../UI/Main/MainPopup.hpp"

using namespace AutoPauseMod::DataManagement;
using namespace AutoPauseMod::Waypoints;
using namespace geode::prelude;



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
        &Waypoint::GetTriggerPercentage
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
            &Waypoint::GetTriggerPercentage
        );

        this->m_loadedGlobalWaypoints.insert(pos, waypoint);

        //we have a new global waypoint, save data
        this->SaveGlobalWaypointInformation();
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
            &Waypoint::GetTriggerPercentage
        );

        this->m_loadedLevelWaypoints.insert(pos, waypoint);
        this->SaveLevelWaypointInformation();
    }
}

void DataManager::DeleteWaypoint(const std::shared_ptr<Waypoint>& waypoint) {
    if (waypoint->IsGlobal()) {
        auto it = std::ranges::find(this->m_loadedGlobalWaypoints, waypoint);
        if (it != this->m_loadedGlobalWaypoints.end()) this->m_loadedGlobalWaypoints.erase(it);

        this->SaveGlobalWaypointInformation();
    }

    else {
        auto it = std::ranges::find(this->m_loadedLevelWaypoints, waypoint);
        if (it != this->m_loadedLevelWaypoints.end()) this->m_loadedLevelWaypoints.erase(it);

        this->SaveLevelWaypointInformation();
    }
}

void DataManager::SetShouldPauseOnNewBest(bool newState) {
    this->m_bPauseOnNewBest = newState;
    Mod::get()->setSavedValue<bool>("pauseOnNewBest", newState);
}

void DataManager::SetAttemptStartPercentage(float percentage) {
    this->m_attemptStartPercent = percentage;
}

void DataManager::DiscardPopup() {
    this->m_menuPopup = nullptr;
}

void DataManager::ShowMenuPopup() const {
    if (this->m_menuPopup)
        this->m_menuPopup->show();
}

void DataManager::DeleteAllWaypoints() {
    this->m_loadedGlobalWaypoints.clear();
    this->m_loadedLevelWaypoints.clear();

    this->SaveGlobalWaypointInformation();
    this->SaveLevelWaypointInformation();
}

void DataManager::SetShouldIgnorePracticeMode(bool newState) {
    this->m_bIgnorePracticeMode = newState;
    Mod::get()->setSavedValue<bool>("ignorePracticeMode", newState);
}

void DataManager::SaveGlobalWaypointInformation() {
    std::lock_guard lock (this->m_waypointSaveLoadOperationMutex);

    DataPersistence::SerialiseAndSaveWaypoints(this->m_loadedGlobalWaypoints);
    log::debug("saved {} global waypoints", this->m_loadedGlobalWaypoints.size());
}

void DataManager::SaveLevelWaypointInformation() {
    if (this->m_currentLevelID == 0) return;
    std::lock_guard lock (this->m_waypointSaveLoadOperationMutex);

    DataPersistence::SerialiseAndSaveWaypoints(this->m_loadedLevelWaypoints, this->m_currentLevelID);
    log::debug("saved {} waypoints for level {}", this->m_loadedLevelWaypoints.size(), this->m_currentLevelID, this->m_bIsEditorLevel);
}

void DataManager::UpdateForLevelInformation(GJGameLevel* level) {
    std::lock_guard lock (this->m_waypointSaveLoadOperationMutex);

    if (!level || level->isPlatformer() || LevelEditorLayer::get()) {
        this->m_bIgnoreState = true;
        this->m_currentLevelID = 0;
        log::debug("ignoring unsupported level type.");
        return;
    } else this->m_bIgnoreState = false;

    if (level->m_levelType == GJLevelType::Editor) {
        this->m_currentLevelID = level->m_levelID.value();
        this->m_bIsEditorLevel = false;
    } else {
        this->m_currentLevelID = EditorIDs::getID(level);
        this->m_bIsEditorLevel = true;
    }

    //load waypoints.
    this->m_loadedLevelWaypoints = DataPersistence::LoadLevelWaypoints(this->m_currentLevelID, this->m_bIsEditorLevel);
    log::debug("loaded {} waypoints for level {}", this->m_loadedLevelWaypoints.size(), level->m_levelID);

    this->DiscardPopup();
}

bool DataManager::CheckWaypoints(const float currentPercentage) const {
    const auto shouldPause = [&](const std::shared_ptr<Waypoint>& waypoint) -> bool {
        return waypoint->ShouldPause(this, currentPercentage);
    };

    return std::ranges::any_of(this->m_loadedGlobalWaypoints, shouldPause) ||
        std::ranges::any_of(this->m_loadedLevelWaypoints, shouldPause);
}

void DataManager::UpdateMenuPopupPointer(UI::Main::MainMenuPopup* newPointer) {
    if (!this->m_menuPopup)
        this->m_menuPopup = newPointer;
}

void DataManager::UpdateWaypointListPosition(const std::shared_ptr<Waypoints::Waypoint>& waypoint) {
    auto& waypoints = waypoint->IsGlobal()? this->m_loadedGlobalWaypoints: this->m_loadedLevelWaypoints;

    std::erase(waypoints, waypoint);

    auto pos = std::ranges::lower_bound(
    this->m_loadedGlobalWaypoints,
    waypoint->GetTriggerPercentage(),
    {},
    &Waypoint::GetTriggerPercentage
    );

    waypoints.insert(pos, waypoint);
}

//gonna do some textbook RAII bc uhh its cool :3
//okay but realistically its actually useful
DataManager::DataManager() {
    this->m_loadedGlobalWaypoints = DataPersistence::LoadGlobalWaypoints();
    this->m_bPauseOnNewBest = Mod::get()->getSavedValue<bool>("pauseOnNewBest", true);
    this->m_bIgnorePracticeMode = Mod::get()->getSavedValue<bool>("ignorePracticeMode", true);
}