#pragma once
#include <Geode/Geode.hpp>
#include "../Waypoints/Definitions.hpp"
#include "DataPersistence/Managers.hpp"
#include "../UI/Main/MainPopup.hpp"

namespace AutoPauseMod::UI::Main {
    class MainMenuPopup;
}

namespace AutoPauseMod::DataManagement {
    class DataManager final {
        private:
            geode::WeakRef<UI::Main::MainMenuPopup> m_menuPopup = nullptr;

            //these two will always be sorted
            DataPersistence::WaypointList m_loadedLevelWaypoints {};
            DataPersistence::WaypointList m_loadedGlobalWaypoints {};

            float m_attemptStartPercent = 0;
            int m_currentLevelID = 0;
            bool m_bIsEditorLevel = false;

            bool m_bPauseOnNewBest = true;
            bool m_bIgnorePracticeMode = true;

            //true for any unsupported level type
            bool m_bIgnoreState = false;

            void SetShouldIgnorePracticeMode(bool newState);
            void SetShouldPauseOnNewBest(bool newState);

            DataManager();
            ~DataManager() = default;

            friend class UI::Main::MainMenuPopup;

        public:
            static DataManager* GetSingleton() {
                static DataManager dataManager;
                return &dataManager;
            }

            void UpdateWaypointListPosition(const std::shared_ptr<Waypoints::Waypoint>& waypoint);
            void DeleteAllWaypoints();

            bool GetIgnoreState() const {return this->m_bIgnoreState;}
            int GetCurrentStoredLevelID() const {return this->m_currentLevelID;}

            void ToggleWaypoint(const std::shared_ptr<Waypoints::Waypoint>& waypoint);
            void DeleteWaypoint(const std::shared_ptr<Waypoints::Waypoint>& waypoint);

            bool GetShouldPauseOnNewBest() const {return this->m_bPauseOnNewBest;}
            bool GetIgnorePracticeMode() const {return this->m_bIgnorePracticeMode;}

            bool CheckShouldPauseOnDeath(const float currentPercentage) const;
            std::shared_ptr<Waypoints::Waypoint> NewWaypoint();

            void UpdateForLevelInformation(GJGameLevel* level);
            void SaveLevelWaypointInformation();
            void SaveGlobalWaypointInformation() const;

            float GetAttemptStartPercentage() const {return this->m_attemptStartPercent;}
            void SetAttemptStartPercentage(float percentage);

            void ShowMenuPopup() const;

            void UpdateMenuPopupPointer(geode::WeakRef<UI::Main::MainMenuPopup> newPointer);
            UI::Main::MainMenuPopup* GetMenuPopup() const {return this->m_menuPopup.lock();}

            DataPersistence::WaypointList GetLevelWaypoints() const {return this->m_loadedLevelWaypoints;}
            DataPersistence::WaypointList GetGlobalWaypoints() const {return this->m_loadedGlobalWaypoints;}
    };
}
