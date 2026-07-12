#pragma once

namespace AutoPauseMod::Waypoints {
    class Waypoint;
}

namespace AutoPauseMod::DataManagement::DataPersistence {
    using WaypointList = std::vector<std::shared_ptr<Waypoints::Waypoint>>;

    void SerialiseAndSaveWaypoints(const WaypointList& waypoints, int levelID = 0, bool isEditorLevel = false);
    WaypointList LoadLevelWaypoints(const int levelId, bool isEditorLevel = false);
    WaypointList LoadGlobalWaypoints();
}