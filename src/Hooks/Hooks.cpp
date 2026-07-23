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
        const float currentPercentage = this->getCurrentPercent();

        PlayLayer::destroyPlayer(player, object);

        const bool isNewBest = dataManager->GetAttemptStartPercentage() <= 0.01f &&
            currentPercentage >= 1.0f && (currentPercentage > currentBest) && !this->m_isPracticeMode;

        //pause on: new best + new best setting, or any waypoint signals a pause
        if (isNewBest && dataManager->GetShouldPauseOnNewBest() || dataManager->CheckShouldPauseOnDeath(currentPercentage))
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

    void onResume(CCObject* sender) {
        PauseLayer::onResume(sender);

        if (g_doPauseResetSequence) {
            g_doPauseResetSequence = false;
            PlayLayer::get()->resetLevel();
        }
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