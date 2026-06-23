//
// Created by katie on 22/06/2026.
//

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

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
};