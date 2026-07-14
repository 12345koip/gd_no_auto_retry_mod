#include <Geode/Geode.hpp>
#include "Definitions.hpp"
#include "../DataManagement/DataManager.hpp"

using namespace AutoPauseMod::Waypoints;
using namespace AutoPauseMod::DataManagement;
using namespace geode::prelude;

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

static bool ValidatePercentage(const DataManager* dataManager, const Waypoint* waypoint, const float currentPercentage) {
    const float startPos = dataManager->GetAttemptStartPercentage();
    const float activationPercentage = waypoint->GetTriggerPercentage();

    switch (waypoint->GetBehaviourType()) {
        case WaypointBehaviourType::FromAnywhere:
            return currentPercentage >= activationPercentage || currentPercentage - startPos >= activationPercentage;

        case WaypointBehaviourType::FromStartPosOnly:
            return startPos >= 0.01f && (currentPercentage - startPos >= activationPercentage);

        case WaypointBehaviourType::FromStartOnly:
            return startPos <= 0.01f && currentPercentage >= activationPercentage;

        case WaypointBehaviourType::OnExactPercentage:
            return std::floor(currentPercentage) == std::floor(activationPercentage);
    }

    return false;
}

bool Waypoint::ShouldPause(const DataManager* dataManager, const float currentPercentage) const {
    return this->m_bEnabled && ValidatePercentage(dataManager, this, currentPercentage);
}