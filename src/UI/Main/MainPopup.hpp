//
// Created by katie on 26/06/2026.
//

#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <unordered_map>
#include <optional>

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

            //TODO: add hooks to listen for exiting a level, and have the UI discarded then.

            [[nodiscard]] CCNode* makeUIForWaypoint(const std::shared_ptr<Waypoints::Waypoint>& waypoint);

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

            void onPracticeToggleClicked(CCObject* sender);
            void onNewBestToggleClicked(CCObject* sender);
            void onNewWaypointButtonClicked(CCObject*);
            void onRowGlobalToggleClicked(CCObject* sender);
            void onRowDeleteButtonClicked(CCObject* sender);
            void onRowEnabledToggleClicked(CCObject* sender);
            void onDeleteAllWaypointsButtonClicked(CCObject*);
            void onDisableAllWaypointsButtonClicked(CCObject*);
            void onInfoButtonClicked(CCObject*);

            [[nodiscard]] CCNode* GetWaypointUI(const Waypoints::Waypoint* waypoint) const;
            std::optional<std::weak_ptr<Waypoints::Waypoint>> GetAssociatedWaypoint(CCNode* ui) const;
            CCNode* ResolveWaypointUIRoot(CCNode* current);

            ~MainMenuPopup() override;
    };
}