//
// Created by katie on 22/06/2026.
//

#pragma once
#include <Geode/Geode.hpp>
#include <memory>
#include <vector>
#include "../Waypoints/Definitions.hpp"
#include "DataPersistence/Managers.hpp"

namespace AutoPauseMod::DataManagement {
    class DataManager final {
        private:
            DataPersistence::WaypointList m_loadedLevelWaypoints {};
            DataPersistence::WaypointList m_loadedGlobalWaypoints {};
            float m_attemptStartPercent = 0;
            int m_currentLevelID = 0;

            //this will be set true for any level type that is not supported,
            //such as an editor playtest or platformer mode. Just means we'll
            //ignore all events
            bool m_bIgnoreState = false;




            void RefreshWaypoints(); //will be called after a new level is entered.

            DataManager() = default;
            ~DataManager() = default;

        public:
            static DataManager& GetSingleton() {
                static DataManager dataManager;
                return dataManager;
            }

            std::shared_ptr<Waypoints::Waypoint> NewWaypoint();
            void UpdateLevelInformation();
            [[nodiscard]] float GetAttemptStartPercentage() const {return this->m_attemptStartPercent;}
    };
}
