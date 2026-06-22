//
// Created by katie on 22/06/2026.
//

#include <algorithm>
#include "Definitions.hpp"

using namespace AutoPauseMod::Waypoints;

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
