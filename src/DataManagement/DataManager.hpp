//
// Created by katie on 22/06/2026.
//

#pragma once
#include <Geode/Geode.hpp>
#include <memory>
#include <vector>
#include <mutex>
#include "../Waypoints/Definitions.hpp"
#include "DataPersistence/Managers.hpp"

namespace AutoPauseMod::UI::Main {
    class MainMenuPopup;
}

namespace AutoPauseMod::DataManagement {
    class DataManager final {
        private:

            //this is just an observer pointer bc the user
            //might open it multiple times on one pause
            //and idk the adding of nodes each time might be expensive
            UI::Main::MainMenuPopup* m_menuPopup = nullptr;

            //NOTE: these two waypoint lists should always be in a sorted state
            DataPersistence::WaypointList m_loadedLevelWaypoints {};
            DataPersistence::WaypointList m_loadedGlobalWaypoints {};

            float m_attemptStartPercent = 0;
            int m_currentLevelID = 0;
            bool m_bIsEditorLevel = false;

            bool m_bPauseOnNewBest = true;
            bool m_bIgnorePracticeMode = true;

            //this will be set true for any level type that is not supported,
            //such as an editor playtest or platformer mode. Just means we'll
            //ignore all events
            bool m_bIgnoreState = false;

            std::mutex m_waypointSaveLoadOperationMutex {};


            void RefreshWaypoints(); //will be called after a new level is entered.
            void SetShouldIgnorePracticeMode(bool newState);
            void SetShouldPauseOnNewBest(bool newState);
            void DiscardPopup();


            DataManager();
            ~DataManager() = default;

            friend class AutoPauseMod::UI::Main::MainMenuPopup;

        public:
            static DataManager* GetSingleton() {
                static DataManager dataManager;
                return &dataManager;
            }

            void UpdateWaypointListPosition(const std::shared_ptr<Waypoints::Waypoint>& waypoint);
            void DeleteAllWaypoints();

            [[nodiscard]] bool GetIgnoreState() const {return this->m_bIgnoreState;}
            [[nodiscard]] int GetCurrentStoredLevelID() const {return this->m_currentLevelID;}

            void ToggleWaypoint(const std::shared_ptr<Waypoints::Waypoint>& waypoint);
            void DeleteWaypoint(const std::shared_ptr<Waypoints::Waypoint>& waypoint);

            [[nodiscard]] bool GetShouldPauseOnNewBest() const {return this->m_bPauseOnNewBest;}
            [[nodiscard]] bool GetIgnorePracticeMode() const {return this->m_bIgnorePracticeMode;}

            //performs a check on death to see if any waypoints cause for a pause
            bool CheckWaypoints(const float currentPercentage) const;

            //the default state for a waypoint is always going to be level-specific.
            //the user can toggle it to be global afterwards if they want.
            std::shared_ptr<Waypoints::Waypoint> NewWaypoint();

            void UpdateForLevelInformation(GJGameLevel* level);
            void SaveLevelWaypointInformation();
            void SaveGlobalWaypointInformation();

            [[nodiscard]] float GetAttemptStartPercentage() const {return this->m_attemptStartPercent;}
            void SetAttemptStartPercentage(float percentage);

            void ShowMenuPopup() const;

            void UpdateMenuPopupPointer(UI::Main::MainMenuPopup* newPointer);
            [[nodiscard]] UI::Main::MainMenuPopup* GetMenuPopup() const {return this->m_menuPopup;}

            [[nodiscard]] DataPersistence::WaypointList GetLevelWaypoints() const {return this->m_loadedLevelWaypoints;}
            [[nodiscard]] DataPersistence::WaypointList GetGlobalWaypoints() const {return this->m_loadedGlobalWaypoints;}
    };
}
