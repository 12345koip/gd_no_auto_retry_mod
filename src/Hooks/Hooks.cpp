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

    //hooking PlayLayer::onEnter results in our detour not
    //actually being called, so we will make do here instead.
    void onEnterTransitionDidFinish() override {
        PlayLayer::onEnterTransitionDidFinish();
        const bool isEditorLevel = this->m_level->m_levelType == GJLevelType::Editor;
        log::debug("level enter: {} isEditorLevel: {}", this->m_level->m_levelID, isEditorLevel);
        DataManager::GetSingleton()->UpdateForLevelInformation(this->m_level);
    }

    /*
    NOTE: instead of saving when a level exits (which has proven to be unreliable)
    we only actually need to save when any waypoint information changes,
    so saves are requested from the UI event handlers.

    This isn't going to get horribly expensive as there'll only be as many waypoints
    as the player creates, and listening for exit is inherently unreliable.
    */


    void resetLevel() override {
        PlayLayer::resetLevel();

        auto* DataManager = DataManager::GetSingleton();
        float currentPercent = PlayLayer::getCurrentPercent();
        DataManager->SetAttemptStartPercentage(currentPercent);
        log::debug("new attempt started at {}", currentPercent);
    }

    void destroyPlayer(PlayerObject* player, GameObject* object) override {
        if (object == this->m_anticheatSpike)
            return PlayLayer::destroyPlayer(player, object);



        const auto* DataManager = DataManager::GetSingleton();

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

        if (DataManager::GetSingleton()->GetIgnoreState()) return;

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