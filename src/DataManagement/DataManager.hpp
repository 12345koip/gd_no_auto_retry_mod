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
            //NOTE: these two waypoint lists should always be in a sorted state
            DataPersistence::WaypointList m_loadedLevelWaypoints {};
            DataPersistence::WaypointList m_loadedGlobalWaypoints {};

            float m_attemptStartPercent = 0;
            int m_currentLevelID = 0;

            bool m_bPauseOnNewBest = true;
            bool m_bIgnorePracticeMode = true;

            //this will be set true for any level type that is not supported,
            //such as an editor playtest or platformer mode. Just means we'll
            //ignore all events
            bool m_bIgnoreState = false;




            void RefreshWaypoints(); //will be called after a new level is entered.

            DataManager();
            ~DataManager() = default;

        public:
            static DataManager& GetSingleton() {
                static DataManager dataManager;
                return dataManager;
            }

            [[nodiscard]] bool GetIgnoreState() const {return this->m_bIgnoreState;}

            void ToggleWaypoint(const std::shared_ptr<Waypoints::Waypoint>& waypoint);
            void DeleteWaypoint(const std::shared_ptr<Waypoints::Waypoint>& waypoint);

            [[nodiscard]] bool GetShouldPauseOnNewBest() const {return this->m_bPauseOnNewBest;}
            void SetShouldPauseOnNewBest(bool newState);

            [[nodiscard]] bool GetIgnorePracticeMode() const {return this->m_bIgnorePracticeMode;}
            void SetShouldIgnorePracticeMode(bool newState);

            //performs a check on death to see if any waypoints cause for a pause
            bool CheckWaypoints(const float currentPercentage) const;

            //the default state for a waypoint is always going to be level-specific.
            //the user can toggle it to be global afterwards if they want.
            std::shared_ptr<Waypoints::Waypoint> NewWaypoint();

            void UpdateLevelInformation();
            [[nodiscard]] float GetAttemptStartPercentage() const {return this->m_attemptStartPercent;}
            void SetAttemptStartPercentage(float percentage);
    };
}
