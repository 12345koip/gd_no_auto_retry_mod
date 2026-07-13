#include "MainPopup.hpp"
#include "../Utils.hpp"
#include "../../DataManagement/DataManager.hpp"
#include "../Waypoints/WaypointRow.hpp"

using namespace AutoPauseMod::UI::Main;
using namespace AutoPauseMod::UI::WaypointUI;
using namespace AutoPauseMod::DataManagement;
using namespace geode::prelude;


bool MainMenuPopup::init() {
    if (!Popup::init(400.0f, 250.0f))
        return false;

    this->setTitle("AutoPause On Death Configuration");
    this->setID("MainMenuPopup"_spr);


    auto* DataManager = DataManager::GetSingleton();

    //"ignore practice mode" + "pause on new best" toggles.
    auto toggleMenu = CCMenu::create();
    toggleMenu->setPosition(CCPointZero);
    this->m_mainLayer->addChild(toggleMenu);


    auto toggle_practiceMode = Utils::MakeToggle(
        this,
        menu_selector(MainMenuPopup::onPracticeToggleClicked),
        0.8f,
        {190.0f, 195.0f},
        DataManager->GetIgnorePracticeMode()
    );
    toggleMenu->addChild(toggle_practiceMode);


    auto label_practiceMode = Utils::MakeLabel(
        "Ignore practice mode",
        "goldFont.fnt",
        {0.0f, 0.5f},
        {10.0f, 195.5f},
        0.6f
    );
    this->m_mainLayer->addChild(label_practiceMode);

    //pause on new best toggle
    auto toggle_pauseOnNewBest = Utils::MakeToggle(
        this,
        menu_selector(MainMenuPopup::onNewBestToggleClicked),
        0.8f,
        {375.0f, 195.0f},
        DataManager->GetShouldPauseOnNewBest()
    );

    toggleMenu->addChild(toggle_pauseOnNewBest);

    auto label_pauseOnNewBest = Utils::MakeLabel(
        "Pause on new best",
        "goldFont.fnt",
        {0.0f, 0.5f},
        {215.0f, 195.0f},
        0.6f
    );
    this->m_mainLayer->addChild(label_pauseOnNewBest);





    auto scroller = ScrollLayer::create(
        {280.0f, 125.0f},
        true,
        true
    );
    this->m_scroller = scroller;

    scroller->m_bIgnoreAnchorPointForPosition = false;
    scroller->m_contentLayer->setLayout(
        ScrollLayer::createDefaultListLayout(6.0f)
    );

    this->m_mainLayer->addChildAtPosition(scroller, Anchor::Center, {0.0f, -35.0f});


    //scroller background
    auto bg = CCLayerColor::create({96, 65, 30, 90});
    bg->setContentSize({280.0f, 125.0f});
    bg->m_bIgnoreAnchorPointForPosition = false;

    this->m_mainLayer->addChildAtPosition(bg, Anchor::Center, {0.0f, -35.0f});



    //scroller title
    auto label_waypoints = Utils::MakeLabel(
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
    toggleMenu->addChild(button_new);

    //information button.
    auto button_info = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        this,
        menu_selector(MainMenuPopup::onInfoButtonClicked)
    );
    button_info->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false));
    button_info->m_baseScale = 0.8f;
    button_info->setScale(0.8f);
    button_info->setPosition(353.0f, 143.0f);
    toggleMenu->addChild(button_info);

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
    toggleMenu->addChild(button_deleteAll);


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
    toggleMenu->addChild(button_disableAll);

    this->RebuildWaypointList(
        DataManager->GetGlobalWaypoints(),
        DataManager->GetLevelWaypoints()
    );

    return true;
}





//listeners.

void MainMenuPopup::onNewBestToggleClicked(CCObject* sender) {
    auto toggle = static_cast<CCMenuItemToggler*>(sender);

    bool disabled = toggle->isToggled();
    DataManager::GetSingleton()->SetShouldPauseOnNewBest(!disabled);
}

void MainMenuPopup::onPracticeToggleClicked(CCObject* sender) {
    auto toggle = static_cast<CCMenuItemToggler*>(sender);

    bool disabled = toggle->isToggled();
    DataManager::GetSingleton()->SetShouldIgnorePracticeMode(!disabled);
}

void MainMenuPopup::onNewWaypointButtonClicked(CCObject*) {
    if (auto scroller = this->m_scroller.lock()) {
        auto waypoint = DataManager::GetSingleton()->NewWaypoint();
        auto row = WaypointRow::create(waypoint, this->m_scroller);

        scroller->m_contentLayer->addChild(row);
        scroller->m_contentLayer->updateLayout();
        scroller->scrollToTop();
    }
}

void MainMenuPopup::RebuildWaypointList(const DataPersistence::WaypointList& globalWaypoints, const DataPersistence::WaypointList& levelWaypoints) {
    auto scroller = this->m_scroller.lock();
    if (!scroller) return;

    for (auto* row: scroller->m_contentLayer->getChildrenExt<WaypointRow>())
        row->Delete(false);

    for (const auto& waypoint: globalWaypoints) {
        auto row = WaypointRow::create(waypoint, this->m_scroller);
        scroller->m_contentLayer->addChild(row);
        scroller->m_contentLayer->updateLayout();
    }

    for (const auto& waypoint: levelWaypoints) {
        auto row = WaypointRow::create(waypoint, this->m_scroller);
        scroller->m_contentLayer->addChild(row);
        scroller->m_contentLayer->updateLayout();
    }

    scroller->m_contentLayer->updateLayout();
    scroller->scrollToTop();
}

void MainMenuPopup::onDisableAllWaypointsButtonClicked(CCObject*) {
    geode::createQuickPopup(
        "Confirm",
        "Are you sure you want to disable <cy>all waypoints</c>?",
        "No", "Yes",
        [this](auto, bool didClickYes) -> void {
            if (!didClickYes) return;
            auto scroller = this->m_scroller.lock();

            for (auto* row: scroller->m_contentLayer->getChildrenExt<WaypointRow>())
                row->setEnabled(false);

            auto* DataManager = DataManager::GetSingleton();
            DataManager->SaveGlobalWaypointInformation();
            DataManager->SaveLevelWaypointInformation();
        }
    );
}

void MainMenuPopup::onDeleteAllWaypointsButtonClicked(CCObject*) {
    geode::createQuickPopup(
        "Confirm",
        "Are you sure you want to delete <cy>all waypoints</c>? <cy>This cannot be undone!</c>",
        "No", "Yes",
        [this](auto, bool didClickYes) -> void {
            if (!didClickYes) return;

            auto scroller = this->m_scroller.lock();
            if (!scroller) return;

            scroller->m_contentLayer->removeAllChildren();
            scroller->m_contentLayer->updateLayout();
            scroller->scrollToTop();

            DataManager::GetSingleton()->DeleteAllWaypoints();
        }
    );
}

void MainMenuPopup::onInfoButtonClicked(CCObject*) {
    //WARNING: eyesore ahead, horribly long string literal
    //dont say i didnt warn you

    FLAlertLayer::create(
        nullptr,
        "Waypoint Help",


        "<cj>Waypoints</c> represent a point in the level where if you die, the game should pause. "
        "Waypoints can be <cy>global</c>, or <cy>level-specific</c>, and are level-specific by default."
        "\n\nWaypoints have four different modes, being "
        "<cc>From Start Only (STA), From StartPos Only (SP), On Exact Percentage Only (ONP), and From Anywhere (ANY)</c>."
        "\n\nSee this mod's <cj>About</c> section in the <cy>Geode</c> menu for more information!",


        "OK",
        nullptr,
        350.0f,
        false,
        135.0f,
        0.7f
    )->show();
}