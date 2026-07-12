#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include "../../DataManagement/DataPersistence/Managers.hpp"

namespace AutoPauseMod::Waypoints {
    class Waypoint;
}

namespace AutoPauseMod::UI::Main {

    class MainMenuPopup: public geode::Popup {
        protected:
            bool init() override;

            std::unordered_map<CCNode*, std::weak_ptr<Waypoints::Waypoint>> m_waypointUIMap {};
            geode::ScrollLayer* m_scroller = nullptr;

            CCNode* makeUIForWaypoint(const std::shared_ptr<Waypoints::Waypoint>& waypoint);

        public:
            static MainMenuPopup* create() {
                auto popup = new MainMenuPopup();

                if (popup->init()) {
                    popup->autorelease();
                    return popup;
                }

                delete popup;
                return nullptr;
            }

            void FlushAndRebuildList(const DataManagement::DataPersistence::WaypointList& globalWaypoints, const DataManagement::DataPersistence::WaypointList& levelWaypoints);

            //talk about a lot of listeners...
            void onPracticeToggleClicked(CCObject* sender);
            void onNewBestToggleClicked(CCObject* sender);
            void onNewWaypointButtonClicked(CCObject*);
            void onRowGlobalToggleClicked(CCObject* sender);
            void onRowDeleteButtonClicked(CCObject* sender);
            void onRowEnabledToggleClicked(CCObject* sender);
            void onDeleteAllWaypointsButtonClicked(CCObject*);
            void onDisableAllWaypointsButtonClicked(CCObject*);
            void onInfoButtonClicked(CCObject*);


            CCNode* GetWaypointUI(const Waypoints::Waypoint* waypoint) const;
            std::optional<std::weak_ptr<Waypoints::Waypoint>> GetAssociatedWaypoint(CCNode* ui) const;
            CCNode* ResolveWaypointUIRoot(CCNode* current);

            ~MainMenuPopup() override;
    };
}