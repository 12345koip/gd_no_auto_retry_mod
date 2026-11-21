#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "../DataManagement/DataManager.hpp"
#include "../UI/Main/MainPopup.hpp"

using namespace AutoPauseMod::DataManagement;
using namespace geode::prelude;

//ew a global static
static bool g_doPauseResetSequence = false;

class $modify(ModifiedPlayLayer, PlayLayer) {
    static void onModify(auto& self) {
        if (!self.setHookPriorityAfterPre("PlayLayer::resetLevel", "zilko.death_animations"))
            log::warn("could not set hook priority for PlayLayer::resetLevel");
    }

    void onEnterTransitionDidFinish() override {
        PlayLayer::onEnterTransitionDidFinish();
        g_doPauseResetSequence = false;

        const bool isEditorLevel = this->m_level->m_levelType == GJLevelType::Editor;

        log::debug("level enter: {} isEditorLevel: {}", this->m_level->m_levelID, isEditorLevel);
        DataManager::GetSingleton()->UpdateForLevelInformation(this->m_level);
    }

    void resetLevel() override {
        if (g_doPauseResetSequence) { //block the reset if a waypoint triggered.
            log::debug("pausing game for waypoint");
            PlayLayer::pauseGame(false);
            return;
        }

        PlayLayer::resetLevel();

        auto* dataManager = DataManager::GetSingleton();
        float currentPercent = PlayLayer::getCurrentPercent();
        dataManager->SetAttemptStartPercentage(currentPercent);
        log::debug("new attempt started at {}", currentPercent);
    }

    void destroyPlayer(PlayerObject* player, GameObject* object) override {
        if (object == this->m_anticheatSpike)
            return PlayLayer::destroyPlayer(player, object);



        const auto* dataManager = DataManager::GetSingleton();
        if (dataManager->GetIgnoreState() || (this->m_isPracticeMode && dataManager->GetIgnorePracticeMode()))
            return PlayLayer::destroyPlayer(player, object);


        const int currentBest = this->m_level->getNormalPercent();
        const int currentSessionBest = this->m_level->getSessionBest();
        const float currentPercentage = this->getCurrentPercent();

        PlayLayer::destroyPlayer(player, object);

        const bool isFromStart = dataManager->GetAttemptStartPercentage() <= 0.01f;
        const bool isNewBest = isFromStart && currentPercentage >= 1.0f && (currentPercentage > currentBest) && !this->m_isPracticeMode;
        const bool isNewSessionBest = isFromStart && currentBest >= 1.0f && (currentPercentage > currentSessionBest) && !this->m_isPracticeMode;

        //pause on: new best + new best setting, new session best + setting, or any waypoint signals a pause
        if ((isNewBest && dataManager->GetShouldPauseOnNewBest()) || (isNewSessionBest && dataManager->GetShouldPauseOnNewSessionBest()) || dataManager->CheckShouldPauseOnDeath(currentPercentage))
            g_doPauseResetSequence = true;
    }
};

class $modify(ModifiedPauseLayer, PauseLayer) {
    void customSetup() override {
        PauseLayer::customSetup();

        if (DataManager::GetSingleton()->GetIgnoreState()) {
            log::debug("Ignore state active.");
            return;
        };

        CCNode* menu = this->getChildByID("main-layer")->getChildByID("right-button-menu");
        if (!menu) {
            log::debug("Button menu not found. Skipping");
            return;
        }



        auto icon = CCSprite::create("uiOpenButton_1.png"_spr);
        auto sprite = CircleButtonSprite::create(icon, CircleBaseColor::Green, CircleBaseSize::Medium);

        auto button = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(ModifiedPauseLayer::onUIOpenButtonClicked)
        );

        button->setLayoutOptions(
            AxisLayoutOptions::create()->setAutoScale(false)
        );

        button->m_baseScale = 0.6f;
        button->setScale(0.6f);
        button->setID("ui-open-button"_spr);

        menu->addChild(button);
        menu->updateLayout();
    }

    void onResume(CCObject* sender) {
        log::debug("PauseLayer::onResume called");
        PauseLayer::onResume(sender);

        if (g_doPauseResetSequence) {
            g_doPauseResetSequence = false;
            PlayLayer::get()->resetLevel();
        }
    }

    void onRestart(CCObject* sender) {
        log::debug("PauseLayer::onRestart called");
        g_doPauseResetSequence = false; //must unblock this so the reset goes through.
        PauseLayer::onRestart(sender);
    }

    void onUIOpenButtonClicked(CCObject*) {
        auto* dataManager = DataManager::GetSingleton();

        if (!dataManager->GetMenuPopup()) {
            auto popup = AutoPauseMod::UI::Main::MainMenuPopup::create();
            dataManager->UpdateMenuPopupPointer(popup);
        }

        dataManager->ShowMenuPopup();
    }
};