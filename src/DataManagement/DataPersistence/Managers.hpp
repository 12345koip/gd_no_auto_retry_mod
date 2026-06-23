//
// Created by katie on 23/06/2026.
//

#pragma once
#include <cstdint>
#include <memory>
#include <vector>

namespace AutoPauseMod::Waypoints {
    class Waypoint;
}

namespace AutoPauseMod::DataManagement::DataPersistence {
    //NOTE: while these settings ARE editable by the user (obviously), they are NOT intended to be
    //edited through the Geode settings menu, they will be edited through a CUSTOM UI in the pause menu
    //of the level they are playing!! Hence, we are not using Geode settings here even though
    //it's user-editable data :3

    using WaypointList = std::vector<std::shared_ptr<Waypoints::Waypoint>>;

    struct WaypointInformation { //a waypoint format ready to be saved as JSON
        float activationPercentage;
        uint8_t behaviourType;
        bool enabled;

        //NOTE: not storing LevelID or global.
        //global waypoints will be stored under the key "global" and loaded at startup.
        //level ID waypoints are stored under an array with their associated level ID.

        static WaypointInformation FromWaypoint(const std::shared_ptr<Waypoints::Waypoint> waypoint);
    };

    void SerialiseAndSaveWaypoints(const WaypointList& waypoints);
    WaypointList LoadLevelWaypoints(const int levelId);
    WaypointList LoadGlobalWaypoints();
}