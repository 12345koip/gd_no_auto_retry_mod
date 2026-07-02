//
// Created by katie on 22/06/2026.
//

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "../DataManagement/DataManager.hpp"
#include "../UI/Main/MainPopup.hpp"

using namespace AutoPauseMod::DataManagement;
using namespace geode::prelude;

class $modify(PlayLayer) {
    void resetLevel() override {
        auto* DataManager = DataManager::GetSingleton();

        float currentPercent = PlayLayer::getCurrentPercent();

        DataManager->SetAttemptStartPercentage(currentPercent);
        PlayLayer::resetLevel();
    }



    void destroyPlayer(PlayerObject* player, GameObject* object) override {
        const auto* DataManager = DataManager::GetSingleton();

        if (object == this->m_anticheatSpike)
            return PlayLayer::destroyPlayer(player, object);

        if (DataManager->GetIgnoreState() || (this->m_isPracticeMode && DataManager->GetIgnorePracticeMode()))
            return PlayLayer::destroyPlayer(player, object);

        const int currentBest = this->m_level->getNormalPercent();
        const float currentPercentage = this->getCurrentPercent();
        this->destroyPlayer(player, object);

        const bool isNewBest = (currentPercentage > currentBest) && !this->m_isPracticeMode;

        /*
         * pause on either:
         * new best AND "pause on new best" setting is enabled,
         * OR any enabled waypoint signals a pause
         */
        if ((isNewBest && DataManager->GetShouldPauseOnNewBest()) || DataManager->CheckWaypoints(currentPercentage))
            PlayLayer::pauseGame(false);
    }
};

class $modify(ModifiedPauseLayer, PauseLayer) {
    void customSetup() override {
        PauseLayer::customSetup();

        CCNode* menu = this->getChildByID("bottom-button-menu");
        if (!menu) return;

        auto button = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("AutoPause"),
            this,
            menu_selector(ModifiedPauseLayer::onUIOpenButtonClicked)
        );

        button->setLayoutOptions(
            AxisLayoutOptions::create()
                ->setAutoScale(false)
        );

        button->m_baseScale = 0.7f;
        button->setScale(0.7f);

        menu->addChild(button);
        menu->updateLayout();
    }

    void onUIOpenButtonClicked(CCObject*) {
        auto* DataManager = DataManager::GetSingleton();

        if (!DataManager->GetMenuPopup()) {
            auto popup = AutoPauseMod::UI::Main::MainMenuPopup::create();
            DataManager->UpdateMenuPopupPointer(popup);
        }

        DataManager->ShowMenuPopup();
    }
};