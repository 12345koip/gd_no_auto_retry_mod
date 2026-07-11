//
// Created by katie on 22/06/2026.
//

#include <cmath>
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "../DataManagement/DataManager.hpp"
#include "../UI/Main/MainPopup.hpp"

using namespace AutoPauseMod::DataManagement;
using namespace geode::prelude;

/*
mild crashout here.
literally every useful function for detecting an unpause can't
be hooked through $modify so i have this horrible workaround of
not just one but TWO static global variables AND a per-frame poller
on the PlayLayer.

Not to mention Mod::hook'ing them has weird implications resulting in
the detour never being called too.

welp :3
*/
static bool g_doPauseResetSequence = false;
static bool g_wasPaused = false;



class $modify(ModifiedPlayLayer, PlayLayer) {
    void onEnterTransitionDidFinish() override {
        PlayLayer::onEnterTransitionDidFinish();
        const bool isEditorLevel = this->m_level->m_levelType == GJLevelType::Editor;

        log::debug("level enter: {} isEditorLevel: {}", this->m_level->m_levelID, isEditorLevel);
        DataManager::GetSingleton()->UpdateForLevelInformation(this->m_level);
    }


    void postUpdate(float dt) override {
        PlayLayer::postUpdate(dt);

        if (g_wasPaused && !this->m_isPaused && g_doPauseResetSequence) {
            log::debug("resetting level on unpause");
            g_wasPaused = false;
            g_doPauseResetSequence = false;
            this->resetLevel();
        }
    }

    void resetLevel() override {
        if (g_doPauseResetSequence) { //block the reset if a waypoint triggered.
            g_wasPaused = true;
            log::debug("pausing game for waypoint");
            PlayLayer::pauseGame(false);
            return;
        }

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
        const int currentPercentage = this->getCurrentPercentInt();

        PlayLayer::destroyPlayer(player, object);

        const bool isNewBest = DataManager->GetAttemptStartPercentage() <= 0.01f &&
            currentPercentage >= 1.0f && (currentPercentage > currentBest) && !this->m_isPracticeMode;

        /*
         pause on either:
         new best AND "pause on new best" setting is enabled,
         OR any enabled waypoint signals a pause
        */
        if (isNewBest && DataManager->GetShouldPauseOnNewBest() || DataManager->CheckWaypoints(currentPercentage))
            g_doPauseResetSequence = true;
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
            AxisLayoutOptions::create()->setAutoScale(false)
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