//
// Created by katie on 26/06/2026.
//

#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>

namespace AutoPauseMod::UI::Main {

    class MainMenuPopup: public geode::Popup {
        protected:
            bool init() override;

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

        ~MainMenuPopup() override;
    };
}