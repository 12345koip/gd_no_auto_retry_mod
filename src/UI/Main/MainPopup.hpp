//
// Created by katie on 26/06/2026.
//

#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <unordered_map>

namespace AutoPauseMod::Waypoints {
    class Waypoint;
}

namespace AutoPauseMod::UI::Main {

    class MainMenuPopup: public geode::Popup {
        protected:
            bool init() override;

            //std::weak_ptr won't play nice with std::hash, but this instance
            //as a whole won't persist past exiting the current level.
            //the main thing we wanted to avoid was holding a reference,
            //but a raw pointer really should not be a problem anyway.
            std::unordered_map<const Waypoints::Waypoint*, CCNode*> m_waypointUIMap {};

            //TODO: add hooks to listen for exiting a level, and have the UI discarded then.

            [[nodiscard]] CCNode* makeUIForWaypoint(const std::shared_ptr<Waypoints::Waypoint>& waypoint);

            friend class Waypoints::Waypoint;

            void DiscardUIForWaypoint(const Waypoints::Waypoint* waypoint);

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

            void onPracticeToggleClicked(CCObject* sender);
            void onNewBestToggleClicked(CCObject* sender);
            void onNewWaypointButtonClicked(CCObject*);
            void onRowGlobalToggleClicked(CCObject* sender);
            void onRowDeleteButtonClicked(CCObject* sender);
            void onRowEnabledToggleClicked(CCObject* sender);
            void onDeleteAllWaypointsButtonClicked(CCObject*);
            void onDisableAllWaypointsButtonClicked(CCObject*);

            ~MainMenuPopup() override;
    };
}