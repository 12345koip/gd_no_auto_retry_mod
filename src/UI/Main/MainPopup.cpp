//
// Created by katie on 26/06/2026.
//

#include <Geode/Geode.hpp>
#include "MainPopup.hpp"
#include "../../DataManagement/DataManager.hpp"

using namespace AutoPauseMod::UI::Main;
using namespace AutoPauseMod::DataManagement;
using namespace geode::prelude;

static CCMenuItemToggler* makeToggle(MainMenuPopup* target, SEL_MenuHandler callback, float scale, const CCPoint& position, bool defaultState = false) {
    auto toggle = CCMenuItemToggler::createWithStandardSprites(target, callback, scale);
    toggle->toggle(defaultState);
    toggle->setPosition(position);

    return toggle;
}

static CCLabelBMFont* makeLabel(const char* str, const char* fontFile, const CCPoint& anchorPoint, const CCPoint& position, float scale) {
    auto label = CCLabelBMFont::create(str, fontFile);
    label->setAnchorPoint(anchorPoint);
    label->setPosition(position);
    label->setScale(scale);

    return label;
}

bool MainMenuPopup::init() {
    if (!Popup::init(400.f, 250.f))
        return false;

    this->setTitle("AutoPause On Death Configuration");


    auto& DataManager = DataManager::GetSingleton();

    //--- toggles creation ---//
    auto menu = CCMenu::create();
    menu->setPosition(CCPointZero);
    this->m_mainLayer->addChild(menu);

    auto toggle_practiceMode = makeToggle(
        this,
        menu_selector(MainMenuPopup::onPracticeToggleClicked),
        0.8f,
        {190.f, 195.f},
        DataManager.GetIgnorePracticeMode()
    );
    menu->addChild(toggle_practiceMode);

    auto label_practiceMode = makeLabel(
        "Ignore practice mode",
        "goldFont.fnt",
        {0.0f, 0.5f},
        {10.0f, 195.5f},
        0.6f
    );
    this->m_mainLayer->addChild(label_practiceMode);

    //pause on new best toggle
    auto toggle_pauseOnNewBest = makeToggle(
        this,
        menu_selector(MainMenuPopup::onNewBestToggleClicked),
        0.8f,
        {375.0f, 195.0f},
        DataManager.GetShouldPauseOnNewBest()
    );
    menu->addChild(toggle_pauseOnNewBest);

    auto label_pauseOnNewBest = makeLabel(
        "Pause on new best",
        "goldFont.fnt",
        {0.0f, 0.5f},
        {215.0f, 195.0f},
        0.6f
    );
    this->m_mainLayer->addChild(label_pauseOnNewBest);


    return true;
}



//listeners.
void MainMenuPopup::onNewBestToggleClicked(CCObject* sender) {
    auto toggle = typeinfo_cast<CCMenuItemToggler*>(sender);
    if (!toggle) return;

    bool disabled = toggle->isToggled();
    DataManager::GetSingleton().SetShouldPauseOnNewBest(!disabled);
}

void MainMenuPopup::onPracticeToggleClicked(CCObject* sender) {
    auto toggle = typeinfo_cast<CCMenuItemToggler*>(sender);
    if (!toggle) return;

    bool disabled = toggle->isToggled();
    if (disabled)
        log::debug("toggled on");
    else
        log::debug("toggled off");
    DataManager::GetSingleton().SetShouldIgnorePracticeMode(!disabled);
}