//
// Created by katie on 22/06/2026.
//

#include <algorithm>
#include "Definitions.hpp"
#include "../DataManagement/DataManager.hpp"

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

    if (newGlobal)
        this->m_levelID = 0; //if it was a mistake, the user can click the button again and it'll grab the level ID
}

void Waypoint::SetBehaviourType(WaypointBehaviourType newType) {
    this->m_behaviourType = newType;
}

void Waypoint::SetLevelID(uint64_t newLevelId) {
    this->m_levelID = newLevelId;
}

bool Waypoint::operator==(const std::shared_ptr<Waypoint> other) const {
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
            return startPos <= 0.0f && currentPercentage >= activationPercentage;

        default:
            return false;
    }
}

bool Waypoint::ShouldPause(const DataManager* dataManager, float currentPercentage) const {
    //The waypoint will only be loaded (and hence this function called)
    //if the current level being played matches this target level ID *or*
    //it's a global waypoint. Hence, we don't need to check that here.
    return this->m_bEnabled && ValidatePercentage(dataManager, currentPercentage, this->m_activationPercentage, this->m_behaviourType);
}
