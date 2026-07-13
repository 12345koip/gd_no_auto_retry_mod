#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>

namespace AutoPauseMod::Waypoints {
    class Waypoint;
}

namespace AutoPauseMod::UI::Main {

    class MainMenuPopup: public geode::Popup {
        private:
            bool init() override;
            geode::WeakRef<geode::ScrollLayer> m_scroller = nullptr;

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

            void RebuildWaypointList();

            //talk about a lot of listeners...
            void onPracticeToggleClicked(CCObject* sender);
            void onNewBestToggleClicked(CCObject* sender);
            void onNewWaypointButtonClicked(CCObject*);
            void onDeleteAllWaypointsButtonClicked(CCObject*);
            void onDisableAllWaypointsButtonClicked(CCObject*);
            void onInfoButtonClicked(CCObject*);
    };
}