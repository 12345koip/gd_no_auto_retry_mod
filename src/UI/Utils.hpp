#pragma once
#include <Geode/Geode.hpp>

namespace AutoPauseMod::UI::Utils {
    inline CCMenuItemToggler* MakeToggle(cocos2d::CCNode* target, cocos2d::SEL_MenuHandler callback, float scale, const cocos2d::CCPoint& position, bool defaultState = false) {
        auto toggle = CCMenuItemToggler::createWithStandardSprites(target, callback, scale);
        toggle->toggle(defaultState);
        toggle->setPosition(position);
        toggle->setScale(scale);

        return toggle;
    }

    inline cocos2d::CCLabelBMFont* MakeLabel(const char* str, const char* fontFile, const cocos2d::CCPoint& anchorPoint, const cocos2d::CCPoint& position, float scale) {
        auto label = cocos2d::CCLabelBMFont::create(str, fontFile);
        label->setAnchorPoint(anchorPoint);
        label->setPosition(position);
        label->setScale(scale);

        return label;
    }
}