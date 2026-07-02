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


    auto* DataManager = DataManager::GetSingleton();

    //--- toggles creation ---//
    auto menu = CCMenu::create();
    menu->setPosition(CCPointZero);
    this->m_mainLayer->addChild(menu);

    auto toggle_practiceMode = makeToggle(
        this,
        menu_selector(MainMenuPopup::onPracticeToggleClicked),
        0.8f,
        {190.f, 195.f},
        DataManager->GetIgnorePracticeMode()
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
        DataManager->GetShouldPauseOnNewBest()
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


    //--- scroller with waypoint toggles setup ---//
    auto scroller = ScrollLayer::create(
        {280.0f, 125.0f},
        true,
        true
    );


    scroller->m_bIgnoreAnchorPointForPosition = false;
    scroller->m_contentLayer->setLayout(
        ScrollLayer::createDefaultListLayout(6.0f)
    );

    scroller->setID("element_scroller"_spr);

    this->m_mainLayer->addChildAtPosition(scroller, Anchor::Center, {0.0f, -35.0f});

    auto bg = CCLayerColor::create({0, 0, 0, 90});
    bg->setContentSize({280.0f, 125.0f});
    bg->m_bIgnoreAnchorPointForPosition = false;

    this->m_mainLayer->addChildAtPosition(bg, Anchor::Center, {0.0f, -35.0f});

    auto label_waypoints = makeLabel(
        "Waypoints",
        "bigFont.fnt",
        {0.5f, 0.5f},
        {200.0f, 167.0f},
        0.6f
    );

    this->m_mainLayer->addChild(label_waypoints);



    auto button_new = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("New"),
        this,
        menu_selector(MainMenuPopup::onNewWaypointButtonClicked)
    );

    button_new->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false));
    button_new->m_baseScale = 0.7f;
    button_new->setScale(0.7f);
    button_new->setPosition({317.0f, 166.0f});
    menu->addChild(button_new);


    return true;
}

CCNode* MainMenuPopup::makeUIForWaypoint(const std::shared_ptr<Waypoints::Waypoint>& waypoint) {
    auto row = CCNode::create();
    row->setContentSize({270.0f, 34.0f});

    row->setLayoutOptions(
        AxisLayoutOptions::create()->setLength(34.0f)
    );

    auto bg = CCLayerColor::create({255, 255, 255, 20});
    bg->setContentSize(row->getContentSize());
    bg->m_bIgnoreAnchorPointForPosition = false;
    bg->setAnchorPoint({0.5f, 0.5f});
    bg->setPosition(row->getContentSize() / 2);
    row->addChild(bg);


    //"global" label.
    auto label_global = makeLabel(
        "Global",
        "bigFont.fnt",
        {0.0f, 0.5f},
        {8.0f, 17.0f},
        0.5f
    );
    label_global->setVisible(false); //TODO: remove this when done debugging sob

    row->addChild(label_global);

    //toggle for global
    auto toggle = makeToggle(
        this,
        menu_selector(MainMenuPopup::onRowGlobalToggleClicked),
        0.65f,
        {82.0f, 17.0f},
        waypoint->IsGlobal()
    );

    //toggler menu
    auto toggleMenu = CCMenu::create();
    toggleMenu->setPosition({135.0f, 0.0f});
    toggleMenu->addChild(toggle);
    row->addChild(toggleMenu);

    //percentage trigger input
    auto input = TextInput::create(55.0f, "100");
    input->setPosition({30.0f, 17.0f});
    input->setScale(0.7f);
    input->setAnchorPoint({0.5f, 0.5f});
    row->addChild(input);

    //percentage sign
    auto label_percentage = makeLabel(
        "%",
        "bigFont.fnt",
        {0.0f, 0.5f},
        {55.0f, 17.0f},
        0.5f
    );
    row->addChild(label_percentage);

    return row;
}

MainMenuPopup::~MainMenuPopup() {
    DataManager::GetSingleton()->DiscardPopup();
}

//listeners.
void MainMenuPopup::onNewBestToggleClicked(CCObject* sender) {
    auto toggle = typeinfo_cast<CCMenuItemToggler*>(sender);
    if (!toggle) return;

    bool disabled = toggle->isToggled();
    DataManager::GetSingleton()->SetShouldPauseOnNewBest(!disabled);
}

void MainMenuPopup::onPracticeToggleClicked(CCObject* sender) {
    auto toggle = typeinfo_cast<CCMenuItemToggler*>(sender);
    if (!toggle) return;

    bool disabled = toggle->isToggled();
    DataManager::GetSingleton()->SetShouldIgnorePracticeMode(!disabled);
}

void MainMenuPopup::onNewWaypointButtonClicked(CCObject*) {
    auto test_wp = std::make_shared<Waypoints::Waypoint>(
        Waypoints::WaypointBehaviourType::FromAnywhere,
        0.0f,
        0.0f
    );

    CCNode* row = MainMenuPopup::makeUIForWaypoint(test_wp);
    auto scroller = static_cast<geode::ScrollLayer*>(
        this->m_mainLayer->getChildByID("element_scroller"_spr)
    );

    scroller->m_contentLayer->addChild(row);
    scroller->m_contentLayer->updateLayout();
    scroller->scrollToTop();
}

void MainMenuPopup::onRowGlobalToggleClicked(CCObject* sender) {
    log::debug("click");
}