//
// Created by katie on 22/06/2026.
//

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "../DataManagement/DataManager.hpp"

using namespace AutoPauseMod::DataManagement;
using namespace geode::prelude;

class $modify(PlayLayer) {
    void resetLevel() override {
        PlayLayer* PlayLayer = PlayLayer::get();

        float currentPercent = PlayLayer->getCurrentPercent();
        log::debug("Reset at %.2f", currentPercent);

        PlayLayer->resetLevel();
    }

    void onExit() override {
        
    }

    void destroyPlayer(PlayerObject* player, GameObject* object) override {
        auto* playLayer = PlayLayer::get();
        auto& DataManager = DataManager::GetSingleton();

        if (DataManager.GetIgnoreState()) return playLayer->destroyPlayer(player, object);

        const int currentBest = playLayer->m_level->getNormalPercent();
        const float currentPercentage = playLayer->getCurrentPercent();
        playLayer->destroyPlayer(player, object);

        const bool isNewBest = currentPercentage > currentBest;

        /*
         * pause on either:
         * new best AND "pause on new best" setting is enabled,
         * OR any enabled waypoint signals a pause
         */
        if ((isNewBest && DataManager.GetShouldPauseOnNewBest()) || DataManager.CheckWaypoints(currentPercentage))
            playLayer->pauseGame(false);
    }
};