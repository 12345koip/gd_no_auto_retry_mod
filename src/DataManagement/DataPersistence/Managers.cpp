//
// Created by katie on 23/06/2026.
//

#include <Geode/Geode.hpp>
#include <string>
#include "Managers.hpp"
#include "../../Waypoints/Definitions.hpp"

using namespace AutoPauseMod::DataManagement::DataPersistence;
using namespace AutoPauseMod::DataManagement;
using namespace geode::prelude;

WaypointInformation WaypointInformation::FromWaypoint(const std::shared_ptr<Waypoints::Waypoint>& waypoint) {
    return WaypointInformation {
        waypoint->GetTriggerPercentage(),
        static_cast<uint8_t>(waypoint->GetBehaviourType()),
        waypoint->IsEnabled()
    };
}

template<> struct matjson::Serialize<WaypointInformation> {
    static Result<WaypointInformation> fromJson(const matjson::Value& value) {
        //NOTE: compiler might complain about loss of precision, but we stored a uint8_t,
        //we're getting a uint8_t
        GEODE_UNWRAP_INTO(uint8_t behaviourType, value["behaviourType"].asUInt());

        GEODE_UNWRAP_INTO(bool enabled, value["enabled"].asBool());

        //again. we stored a float, we're getting a float
        GEODE_UNWRAP_INTO(float activateAt, value["activateAt"].asDouble());

        return Ok(WaypointInformation {activateAt, behaviourType, enabled});
    }

    static matjson::Value toJson(const WaypointInformation& value) {
        auto json = matjson::Value();

        json["behaviourType"] = value.behaviourType;
        json["enabled"] = value.enabled;
        json["activateAt"] = value.activationPercentage;

        return json;
    }
};



WaypointList DataPersistence::LoadLevelWaypoints(const int levelId, bool isEditorLevel) {
    const std::string levelKey = !isEditorLevel? std::to_string(levelId): std::to_string(levelId) + "e";
    auto waypointInfoList = Mod::get()->getSavedValue<std::vector<WaypointInformation>>(levelKey, {});

    WaypointList waypoints {};
    waypoints.reserve(waypointInfoList.size());

    for (const auto& info: waypointInfoList)
        waypoints.push_back(Waypoints::Waypoint::FromWaypointInformation(levelId, info));

    return waypoints;
}

WaypointList DataPersistence::LoadGlobalWaypoints() {
    auto waypointInfoList = Mod::get()->getSavedValue<std::vector<WaypointInformation>>("global", {});

    WaypointList waypoints {};
    waypoints.reserve(waypointInfoList.size());

    for (const auto& info: waypointInfoList)
        waypoints.push_back(Waypoints::Waypoint::FromWaypointInformation(info));

    return waypoints;
}

void DataPersistence::SerialiseAndSaveWaypoints(const WaypointList& waypoints, int levelID, bool isEditorLevel) {
    const bool isGlobal = levelID == 0;


    std::string levelKey {};
    if (isGlobal)
        levelKey = "global";
    else
        levelKey = isEditorLevel? std::to_string(levelID) + "e": std::to_string(levelID);


    std::vector<WaypointInformation> waypointInfos {};
    waypointInfos.reserve(waypoints.size());

    for (const auto& waypoint: waypoints)
        waypointInfos.push_back(WaypointInformation::FromWaypoint(waypoint));

    Mod::get()->setSavedValue<std::vector<WaypointInformation>>(
        isGlobal? "global": std::to_string(levelID),
        waypointInfos
    );
}
