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
    this->SaveLevelWaypointInformation();

    return waypoint;
}

void DataManager::ToggleWaypoint(const std::shared_ptr<Waypoint>& waypoint) {
    if (!waypoint->IsGlobal()) {
        const auto it = std::ranges::find(this->m_loadedLevelWaypoints, waypoint);
        if (it == this->m_loadedLevelWaypoints.end()) {
            log::warn("waypoint not found in level waypoint list when global toggling");
            return;
        }

        this->m_loadedLevelWaypoints.erase(it);

        waypoint->SetGlobal(true);

        const auto pos = std::ranges::lower_bound(
            this->m_loadedGlobalWaypoints,
            waypoint->GetTriggerPercentage(),
            std::ranges::less{},
            &Waypoint::GetTriggerPercentage
        );

        this->m_loadedGlobalWaypoints.insert(pos, waypoint);
    }

    else {
        const auto it = std::ranges::find(this->m_loadedGlobalWaypoints, waypoint);
        if (it == this->m_loadedGlobalWaypoints.end()) {
            log::warn("waypoint not found in global waypoint list when global toggling");
            return;
        }

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
    }

    this->SaveGlobalWaypointInformation();
    this->SaveLevelWaypointInformation();
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
    if (this->m_currentLevelID == 0 && !this->m_bIsEditorLevel) return;
    std::lock_guard lock (this->m_waypointSaveLoadOperationMutex);

    DataPersistence::SerialiseAndSaveWaypoints(this->m_loadedLevelWaypoints, this->m_currentLevelID, this->m_bIsEditorLevel);
    log::debug("save request for level {} isEditorLevel: {}", this->m_currentLevelID, this->m_bIsEditorLevel);
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

    if (level->m_levelType == GJLevelType::Editor) { //only playtesting is supported for editor levels.
        this->m_currentLevelID = EditorIDs::getID(level);
        this->m_bIsEditorLevel = true;
        log::debug("entered level is editor level; editor level ID: {}", this->m_currentLevelID);
    } else {
        this->m_currentLevelID = level->m_levelID.value();
        this->m_bIsEditorLevel = false;
        log::debug("entered level is not an editor level; level ID: {}", this->m_currentLevelID);
    }

    //load waypoints.
    this->m_loadedLevelWaypoints = DataPersistence::LoadLevelWaypoints(this->m_currentLevelID, this->m_bIsEditorLevel);
    log::debug("loaded {} waypoints for level {}", this->m_loadedLevelWaypoints.size(), this->m_currentLevelID);

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
        waypoints,
        waypoint->GetTriggerPercentage(),
        {},
        &Waypoint::GetTriggerPercentage
    );

    waypoints.insert(pos, waypoint);

    if (waypoint->IsGlobal())
        this->SaveGlobalWaypointInformation();
    else
        this->SaveLevelWaypointInformation();
}

//gonna do some textbook RAII bc uhh its cool :3
//okay but realistically its actually useful
DataManager::DataManager() {
    this->m_loadedGlobalWaypoints = DataPersistence::LoadGlobalWaypoints();
    this->m_bPauseOnNewBest = Mod::get()->getSavedValue<bool>("pauseOnNewBest", true);
    this->m_bIgnorePracticeMode = Mod::get()->getSavedValue<bool>("ignorePracticeMode", true);
}