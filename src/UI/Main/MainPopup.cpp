//
// Created by katie on 26/06/2026.
//

#include <Geode/Geode.hpp>
#include "MainPopup.hpp"
#include "../../DataManagement/DataManager.hpp"
#include "../Selectors/ThreeWaySelector.hpp"

using namespace AutoPauseMod::UI::Main;
using namespace AutoPauseMod::DataManagement;
using namespace geode::prelude;

#define WAYPOINT_SCROLLER_NAME "waypoint_scroller"_spr

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

    scroller->setID(WAYPOINT_SCROLLER_NAME);

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


    //new waypoint button.
    auto button_new = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("New"),
        this,
        menu_selector(MainMenuPopup::onNewWaypointButtonClicked)
    );

    button_new->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false));
    button_new->m_baseScale = 0.7f;
    button_new->setScale(0.7f);
    button_new->setPosition({33.0f, 143.0f});
    menu->addChild(button_new);


    //delete all waypoints button.
    auto button_deleteAll = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png"),
        this,
        menu_selector(MainMenuPopup::onDeleteAllWaypointsButtonClicked)
    );

    button_deleteAll->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false));
    button_deleteAll->m_baseScale = 0.7f;
    button_deleteAll->setScale(0.7f);
    button_deleteAll->setPosition(33.0f, 115.0f);
    menu->addChild(button_deleteAll);


    //disable all waypoints button.
    auto button_disableAll = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
        this,
        menu_selector(MainMenuPopup::onDisableAllWaypointsButtonClicked)
    );

    button_disableAll->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false));
    button_disableAll->m_baseScale = 0.7f;
    button_disableAll->setScale(0.7f);
    button_disableAll->setPosition(33.0f, 82.0f);
    menu->addChild(button_disableAll);

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


    //"global" label + toggle
    auto label_global = makeLabel(
        "Global",
        "bigFont.fnt",
        {0.0f, 0.5f},
        {135.0f, 17.0f},
        0.5f
    );

    label_global->setScale(0.4f);
    row->addChild(label_global);

    auto toggle_global = makeToggle(
        this,
        menu_selector(MainMenuPopup::onRowGlobalToggleClicked),
        0.65f,
        {53.0f, 17.0f},
        waypoint->IsGlobal()
    );

    //"enabled" label + toggle
    auto label_enabled = makeLabel(
        "Enabled",
        "bigFont.fnt",
        {0.0f, 0.5f},
        {58.0f, 17.0f},
        0.5f
    );
    label_enabled->setScale(0.4f);
    row->addChild(label_enabled);

    auto toggle_enabled = makeToggle(
        this,
        menu_selector(MainMenuPopup::onRowEnabledToggleClicked),
        0.65f,
        {-15.0f, 17.0f},
        waypoint->IsEnabled()
    );



    //toggler menu
    auto toggleMenu = CCMenu::create();
    toggleMenu->setPosition({135.0f, 0.0f});
    toggleMenu->addChild(toggle_enabled);
    toggleMenu->addChild(toggle_global);
    row->addChild(toggleMenu);




    //percentage trigger input box + percentage label
    int currentPercent = PlayLayer::get()->getCurrentPercentInt();
    auto input = TextInput::create(55.0f, std::to_string(currentPercent));
    input->setPosition({22.0f, 17.0f});
    input->setScale(0.7f);
    input->setAnchorPoint({0.5f, 0.5f});
    row->addChild(input);

    auto label_percentage = makeLabel(
        "%",
        "bigFont.fnt",
        {0.0f, 0.5f},
        {43.0f, 17.0f},
        0.5f
    );
    row->addChild(label_percentage);

    //delete button
    auto button_deleteWaypoint = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png"),
        this,
        menu_selector(MainMenuPopup::onRowDeleteButtonClicked)
    );

    button_deleteWaypoint->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false));
    button_deleteWaypoint->m_baseScale = 0.4f;
    button_deleteWaypoint->setScale(0.4f);
    button_deleteWaypoint->setPosition(124.0f, 17.0f);
    button_deleteWaypoint->setZOrder(2);
    toggleMenu->addChild(button_deleteWaypoint);


    //waypoint type toggle + label
    auto label_from = makeLabel(
        "From",
        "bigFont.fnt",
        {0.5f, 0.5f},
        {222.0f, 28.0f},
        0.3f
    );
    row->addChild(label_from);


    //selector.
    auto selector = Selectors::ThreeWaySelector::create();
    selector->setPosition({222.0f, 13.0f});
    selector->setScale(0.5f);
    row->addChild(selector);


    //TODO: figure out how lambda captures and referencing
    //will be handled for waypoints

    return row;
}

void MainMenuPopup::DiscardUIForWaypoint(const Waypoints::Waypoint* waypoint) {
    const auto it = this->m_waypointUIMap.find(waypoint);

    if (it != this->m_waypointUIMap.end()) {
        it->second->removeFromParent();
        this->m_waypointUIMap.erase(it);
    }
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
        this->m_mainLayer->getChildByID(WAYPOINT_SCROLLER_NAME)
    );

    scroller->m_contentLayer->addChild(row);
    scroller->m_contentLayer->updateLayout();
    scroller->scrollToTop();
}

void MainMenuPopup::onRowGlobalToggleClicked(CCObject* sender) {
    log::debug("click");
}

void MainMenuPopup::onDisableAllWaypointsButtonClicked(CCObject*) {
    geode::createQuickPopup(
        "Confirm",
        "Are you sure you want to disable <cy>all waypoints</c>?",
        "Yes", "No",
        [](auto, bool didClickNo) -> void {
            if (didClickNo) log::debug("no");
        }
    );
}

void MainMenuPopup::onDeleteAllWaypointsButtonClicked(CCObject*) {
    geode::createQuickPopup(
        "Confirm",
        "Are you sure you want to delete <cy>all waypoints</c>? <cy>This cannot be undone!</c>",
        "Yes", "No",
        [](auto, bool didClickNo) -> void {
            if (didClickNo) log::debug("no");
        }
    );
}

void MainMenuPopup::onRowDeleteButtonClicked(CCObject* sender) {
    log::debug("click delete on row");
}

void MainMenuPopup::onRowEnabledToggleClicked(CCObject* sender) {

}
