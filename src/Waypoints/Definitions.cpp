//
// Created by katie on 22/06/2026.
//

#include <algorithm>
#include "Definitions.hpp"
#include "../DataManagement/DataManager.hpp"
#include "../DataManagement/DataPersistence/Managers.hpp"
#include "../UI/Main/MainPopup.hpp"

using namespace AutoPauseMod::Waypoints;
using namespace AutoPauseMod::DataManagement;

void Waypoint::SetTriggerPercentage(float newPercentage) {
    this->m_activationPercentage = std::clamp(newPercentage, 0.0f, 100.0f);
}

void Waypoint::SetEnabled(bool newEnabled) {
    this->m_bEnabled = newEnabled;
}

void Waypoint::SetGlobal(bool newGlobal) {
    this->m_bIsGlobal = newGlobal;
}

void Waypoint::SetBehaviourType(WaypointBehaviourType newType) {
    this->m_behaviourType = newType;
}

void Waypoint::SetLevelID(int newLevelId) {
    this->m_levelID = newLevelId;
}

bool Waypoint::operator==(const std::shared_ptr<Waypoint>& other) const {
    return (
        this->m_levelID == other->GetLevelID() &&
        this->m_bIsGlobal == other->IsGlobal() &&
        this->m_bEnabled == other->IsEnabled() &&
        this->m_activationPercentage == other->GetTriggerPercentage() &&
        this->m_behaviourType == other->GetBehaviourType()
    );
}

static bool ValidatePercentage(const DataManager* dataManager, const float currentPercentage, const float activationPercentage, const WaypointBehaviourType behaviourType) {
    //from anywhere - you reach a certain percentage in the level OR get a certain percentage relative to your start position
    //from start pos only - you reach a certain percentage relative to your start position
    //from start only - you reach a certain percentage from zero

    const float startPos = dataManager->GetAttemptStartPercentage();

    switch (behaviourType) {
        case WaypointBehaviourType::FromAnywhere:
            return currentPercentage >= activationPercentage || currentPercentage - startPos >= activationPercentage;

        case WaypointBehaviourType::FromStartPosOnly:
            return currentPercentage - startPos >= activationPercentage;

        case WaypointBehaviourType::FromStartOnly:
            return startPos <= 0.01f && currentPercentage >= activationPercentage;

        default:
            return false;
    }
}

bool Waypoint::ShouldPause(const DataManager* dataManager, const float currentPercentage) const {
    //The waypoint will only be loaded (and hence this function called)
    //if the current level being played matches this target level ID *or*
    //it's a global waypoint. Hence, we don't need to check that here.
    return this->m_bEnabled && ValidatePercentage(dataManager, currentPercentage, this->m_activationPercentage, this->m_behaviourType);
}

std::shared_ptr<Waypoint> Waypoint::FromWaypointInformation(const int levelId, const DataPersistence::WaypointInformation& info) {
    auto waypoint = std::make_shared<Waypoint>(
        static_cast<WaypointBehaviourType>(info.behaviourType),
        info.activationPercentage,
        0
    );

    waypoint->SetLevelID(levelId);
    waypoint->SetEnabled(info.enabled);

    return waypoint;
}

std::shared_ptr<Waypoint> Waypoint::FromWaypointInformation(const DataPersistence::WaypointInformation& info) {
    auto waypoint = std::make_shared<Waypoint>(
        static_cast<WaypointBehaviourType>(info.behaviourType),
        info.activationPercentage,
        0
    );

    waypoint->SetGlobal(true);
    waypoint->SetEnabled(info.enabled);

    return waypoint;
}

Waypoint::~Waypoint() {
    if (auto* popup = DataManager::GetSingleton()->GetMenuPopup())
        popup->DiscardUIForWaypoint(this);
}