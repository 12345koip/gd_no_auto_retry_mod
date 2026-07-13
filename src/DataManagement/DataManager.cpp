#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include <utility>
#include "DataManager.hpp"
#include "../UI/Main/MainPopup.hpp"

using namespace AutoPauseMod::DataManagement;
using namespace AutoPauseMod::Waypoints;
using namespace geode::prelude;


std::shared_ptr<Waypoint> DataManager::NewWaypoint() {
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
    const bool wasGlobal = waypoint->IsGlobal();
    auto& from = wasGlobal? this->m_loadedGlobalWaypoints: this->m_loadedLevelWaypoints;
    auto& to = wasGlobal? this->m_loadedLevelWaypoints: this->m_loadedGlobalWaypoints;

    const auto it = std::ranges::find(from, waypoint);
    if (it == from.end()) {
        log::warn("waypoint not found in {} list when toggling", wasGlobal? "global": "level");
        return;
    }

    from.erase(it);
    waypoint->SetGlobal(!wasGlobal);

    if (wasGlobal)
        waypoint->SetLevelID(this->m_currentLevelID);

    const auto insertPos = std::ranges::lower_bound(
        to,
        waypoint->GetTriggerPercentage(),
        std::ranges::less{},
        &Waypoint::GetTriggerPercentage
    );
    to.insert(insertPos, waypoint);

    this->SaveGlobalWaypointInformation();
    this->SaveLevelWaypointInformation();
}

void DataManager::DeleteWaypoint(const std::shared_ptr<Waypoint>& waypoint) {
    auto& waypointList = waypoint->IsGlobal()? this->m_loadedGlobalWaypoints: this->m_loadedLevelWaypoints;

    const auto it = std::ranges::find(waypointList, waypoint);
    if (it != waypointList.end())
        waypointList.erase(it);


    if (waypoint->IsGlobal())
        this->SaveGlobalWaypointInformation();
    else
        this->SaveLevelWaypointInformation();
}

void DataManager::SetShouldPauseOnNewBest(bool newState) {
    this->m_bPauseOnNewBest = newState;
    Mod::get()->setSavedValue<bool>("pauseOnNewBest", newState);
}

void DataManager::SetAttemptStartPercentage(float percentage) {
    this->m_attemptStartPercent = percentage;
}

void DataManager::ShowMenuPopup() const {
    if (auto popup = this->m_menuPopup.lock())
        popup->show();
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

void DataManager::SaveGlobalWaypointInformation() const {
    DataPersistence::SerialiseAndSaveWaypoints(this->m_loadedGlobalWaypoints);
    log::debug("saved {} global waypoints", this->m_loadedGlobalWaypoints.size());
}

void DataManager::SaveLevelWaypointInformation() {
    if (this->m_currentLevelID == 0 && !this->m_bIsEditorLevel) return;

    DataPersistence::SerialiseAndSaveWaypoints(this->m_loadedLevelWaypoints, this->m_currentLevelID, this->m_bIsEditorLevel);
    log::debug("save request for level {} isEditorLevel: {}", this->m_currentLevelID, this->m_bIsEditorLevel);
    log::debug("saved {} waypoints for level {}", this->m_loadedLevelWaypoints.size(), this->m_currentLevelID, this->m_bIsEditorLevel);
}

void DataManager::UpdateForLevelInformation(GJGameLevel* level) {
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
}

bool DataManager::CheckShouldPauseOnDeath(const float currentPercentage) const {
    const auto shouldPause = [&](const std::shared_ptr<Waypoint>& waypoint) -> bool {
        return waypoint->ShouldPause(this, currentPercentage);
    };

    return std::ranges::any_of(this->m_loadedGlobalWaypoints, shouldPause) ||
        std::ranges::any_of(this->m_loadedLevelWaypoints, shouldPause);
}

void DataManager::UpdateMenuPopupPointer(geode::WeakRef<UI::Main::MainMenuPopup> newPointer) {
    if (!this->m_menuPopup)
        this->m_menuPopup = std::move(newPointer);
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

DataManager::DataManager() {
    this->m_loadedGlobalWaypoints = DataPersistence::LoadGlobalWaypoints();
    this->m_bPauseOnNewBest = Mod::get()->getSavedValue<bool>("pauseOnNewBest", true);
    this->m_bIgnorePracticeMode = Mod::get()->getSavedValue<bool>("ignorePracticeMode", true);
}