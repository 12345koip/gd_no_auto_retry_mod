//
// Created by katie on 22/06/2026.
//

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "../DataManagement/DataManager.hpp"

using namespace AutoPauseMod::DataManagement;
using namespace geode::prelude;

class $modify(PlayLayer) {
    void resetLevel() override {
        PlayLayer* PlayLayer = PlayLayer::get();
        auto& DataManager = DataManager::GetSingleton();

        float currentPercent = PlayLayer->getCurrentPercent();

        DataManager.SetAttemptStartPercentage(currentPercent);
        PlayLayer->resetLevel();
    }



    void destroyPlayer(PlayerObject* player, GameObject* object) override {
        auto* playLayer = PlayLayer::get();
        const auto& DataManager = DataManager::GetSingleton();

        if (DataManager.GetIgnoreState() || playLayer->m_isPracticeMode && DataManager.GetIgnorePracticeMode())
            return playLayer->destroyPlayer(player, object);

        const int currentBest = playLayer->m_level->getNormalPercent();
        const float currentPercentage = playLayer->getCurrentPercent();
        playLayer->destroyPlayer(player, object);

        const bool isNewBest = (currentPercentage > currentBest) && !playLayer->m_isPracticeMode;

        /*
         * pause on either:
         * new best AND "pause on new best" setting is enabled,
         * OR any enabled waypoint signals a pause
         */
        if ((isNewBest && DataManager.GetShouldPauseOnNewBest()) || DataManager.CheckWaypoints(currentPercentage))
            playLayer->pauseGame(false);
    }
};

class $modify(PauseLayer) {
    void customSetup() override {
        PauseLayer::customSetup();

    }
};