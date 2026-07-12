#include <Geode/Geode.hpp>
#include "Managers.hpp"
#include "../../Waypoints/Definitions.hpp"

using namespace AutoPauseMod::DataManagement::DataPersistence;
using namespace AutoPauseMod::DataManagement;
using namespace AutoPauseMod::Waypoints;
using namespace geode::prelude;

template<> struct matjson::Serialize<std::shared_ptr<Waypoint>> {
    static Result<std::shared_ptr<Waypoint>> fromJson(const matjson::Value& value) {
        GEODE_UNWRAP_INTO(uint8_t behaviourType, value["behaviourType"].asUInt());
        GEODE_UNWRAP_INTO(bool enabled, value["enabled"].asBool());
        GEODE_UNWRAP_INTO(float activateAt, value["activateAt"].asDouble());

        return Ok(
            std::make_shared<Waypoint>(
                static_cast<WaypointBehaviourType>(behaviourType),
                activateAt,
                enabled
            )
        );
    }

    static matjson::Value toJson(const std::shared_ptr<Waypoint>& waypoint) {
        auto json = matjson::Value();

        json["behaviourType"] = static_cast<uint8_t>(waypoint->GetBehaviourType());
        json["enabled"] = waypoint->IsEnabled();
        json["activateAt"] = waypoint->GetTriggerPercentage();

        return json;
    }
};

WaypointList DataPersistence::LoadLevelWaypoints(const int levelId, bool isEditorLevel) {
    //editor level IDs are appended with "e"
    const std::string levelKey = isEditorLevel? std::to_string(levelId) + "e": std::to_string(levelId);
    log::debug("processing load request. levelKey: {}", levelKey);

    auto waypointList = Mod::get()->getSavedValue<WaypointList>(levelKey, {});
    return waypointList;
}

WaypointList DataPersistence::LoadGlobalWaypoints() {
    log::debug("processing load request for global waypoints");

    auto waypointList = Mod::get()->getSavedValue<WaypointList>("global", {});
    return waypointList;
}

void DataPersistence::SerialiseAndSaveWaypoints(const WaypointList& waypoints, int levelID, bool isEditorLevel) {
    const bool isGlobal = levelID == 0;


    std::string levelKey {};
    if (isGlobal)
        levelKey = "global";
    else
        levelKey = isEditorLevel? std::to_string(levelID) + "e": std::to_string(levelID);

    log::debug("processing save request. levelKey: {}", levelKey);
    Mod::get()->setSavedValue<WaypointList>(levelKey, waypoints);
}