#include "MainPopup.hpp"
#include "../../DataManagement/DataManager.hpp"
#include "../Selectors/WaypointBehaviourSelector.hpp"

using namespace AutoPauseMod::UI::Main;
using namespace AutoPauseMod::DataManagement;
using namespace geode::prelude;

#define GENERIC_WAYPOINT_ROW_NAME "WaypointRow"_spr
#define GENERIC_WAYPOINT_ENABLED_TOGGLE_NAME "enabledToggler"_spr
#define GENERIC_WAYPOINT_GLOBAL_TOGGLE_NAME "globalToggler"_spr
#define MAIN_WAYPOINT_MENU_ID "MainMenuPopup"_spr


static CCMenuItemToggler* makeToggle(MainMenuPopup* target, SEL_MenuHandler callback, float scale, const CCPoint& position, bool defaultState = false) {
    auto toggle = CCMenuItemToggler::createWithStandardSprites(target, callback, scale);
    toggle->toggle(defaultState);
    toggle->setPosition(position);
    toggle->setScale(scale);

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
    this->setID(MAIN_WAYPOINT_MENU_ID);


    auto* DataManager = DataManager::GetSingleton();

    //"ignore practice mode" + "pause on new best" toggles.
    auto toggleMenu = CCMenu::create();
    toggleMenu->setPosition(CCPointZero);
    this->m_mainLayer->addChild(toggleMenu);


    auto toggle_practiceMode = makeToggle(
        this,
        menu_selector(MainMenuPopup::onPracticeToggleClicked),
        0.8f,
        {190.f, 195.f},
        DataManager->GetIgnorePracticeMode()
    );
    toggleMenu->addChild(toggle_practiceMode);


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

    toggleMenu->addChild(toggle_pauseOnNewBest);

    auto label_pauseOnNewBest = makeLabel(
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
    auto bg = CCLayerColor::create({0, 0, 0, 90});
    bg->setContentSize({280.0f, 125.0f});
    bg->m_bIgnoreAnchorPointForPosition = false;

    this->m_mainLayer->addChildAtPosition(bg, Anchor::Center, {0.0f, -35.0f});



    //scroller title
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

    this->FlushAndRebuildList(
        DataManager->GetGlobalWaypoints(),
        DataManager->GetLevelWaypoints()
    );

    return true;
}

CCNode* MainMenuPopup::makeUIForWaypoint(const std::shared_ptr<Waypoints::Waypoint>& waypoint) {
    //top-level holder + background
    auto row = CCNode::create();
    row->setContentSize({270.0f, 34.0f});
    row->setLayoutOptions(
        AxisLayoutOptions::create()->setLength(34.0f)
    );

    auto bg = CCLayerColor::create({194, 115, 0, 255});
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
        {132.0f, 17.0f},
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

    toggle_global->setID(GENERIC_WAYPOINT_GLOBAL_TOGGLE_NAME);



    //"enabled" label + toggle
    auto label_enabled = makeLabel(
        "Enabled",
        "bigFont.fnt",
        {0.0f, 0.5f},
        {57.0f, 17.0f},
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

    toggle_enabled->setID(GENERIC_WAYPOINT_ENABLED_TOGGLE_NAME);



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
    input->setString(
        std::to_string(static_cast<int>(waypoint->GetTriggerPercentage())),
        false
    );
    input->setFilter("0123456789");

    row->addChild(input);



    auto label_percentage = makeLabel(
        "%",
        "bigFont.fnt",
        {0.0f, 0.5f},
        {42.0f, 17.0f},
        0.4f
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


    auto behaviourSelector = Selectors::WaypointBehaviourSelector::create();
    behaviourSelector->setPosition({222.0f, 13.0f});
    behaviourSelector->setScale(0.5f);
    behaviourSelector->setState(waypoint->GetBehaviourType());
    row->addChild(behaviourSelector);


    //callbacks.
    behaviourSelector->setCallback(
        [weakWaypoint = std::weak_ptr(waypoint)](const Waypoints::WaypointBehaviourType newBehaviourType) -> void {
            if (weakWaypoint.expired()) {
                log::debug("could not update behaviour type: waypoint expired.");
                return;
            }

            auto waypoint = weakWaypoint.lock();
            waypoint->SetBehaviourType(newBehaviourType);
            log::debug("updated waypoint behaviour type to {}", Waypoints::WaypointTypeToString(newBehaviourType));

            if (waypoint->IsGlobal())
                DataManager::GetSingleton()->SaveGlobalWaypointInformation();
            else
                DataManager::GetSingleton()->SaveLevelWaypointInformation();
        }
    );

    input->setCallback(
        [input, weakWaypoint = std::weak_ptr(waypoint)](const std::string& newText) {
            auto result = geode::utils::numFromString<int>(newText, 10);
            int clampedValue = result? result.unwrap(): 0;
            clampedValue = std::clamp(clampedValue, 0, 100);

            //input text was out of range or else invalid, update input box with safeguard value
            if (auto valueStr = std::to_string(clampedValue); newText != valueStr)
                input->setString(valueStr, false);


            if (auto waypoint = weakWaypoint.lock()) {
                waypoint->SetTriggerPercentage(clampedValue);
                DataManager::GetSingleton()->UpdateWaypointListPosition(waypoint);
            }
        }
    );

    row->setID(GENERIC_WAYPOINT_ROW_NAME);


    this->m_scroller->m_contentLayer->addChild(row);
    this->m_scroller->m_contentLayer->updateLayout();
    this->m_scroller->scrollToTop();


    return row;
}

CCNode* MainMenuPopup::GetWaypointUI(const Waypoints::Waypoint* waypoint) const {
    for (const auto& [ui, otherWaypoint]: this->m_waypointUIMap) {
        if (otherWaypoint.expired()) continue;

        if (otherWaypoint.lock().get() == waypoint)
            return ui;
    }

    return nullptr;
}

std::optional<std::weak_ptr<AutoPauseMod::Waypoints::Waypoint>> MainMenuPopup::GetAssociatedWaypoint(CCNode* ui) const {
    if (this->m_waypointUIMap.contains(ui))
        return this->m_waypointUIMap.at(ui);

    return std::nullopt;
}

MainMenuPopup::~MainMenuPopup() {
    DataManager::GetSingleton()->DiscardPopup();
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
    auto waypoint = DataManager::GetSingleton()->NewWaypoint();

    auto row = makeUIForWaypoint(waypoint);
    this->m_waypointUIMap.insert(std::make_pair(row, waypoint));
}

void MainMenuPopup::FlushAndRebuildList(const DataManagement::DataPersistence::WaypointList& globalWaypoints, const DataManagement::DataPersistence::WaypointList& levelWaypoints) {
    for (const auto& [ui, _wp]: this->m_waypointUIMap)
        ui->removeFromParent();

    this->m_waypointUIMap.clear();

    for (const auto& waypoint: levelWaypoints) {
        auto row = makeUIForWaypoint(waypoint);
        this->m_waypointUIMap.insert(std::make_pair(row, waypoint));
    }

    for (const auto& waypoint: globalWaypoints) {
        auto row = makeUIForWaypoint(waypoint);
        this->m_waypointUIMap.insert(std::make_pair(row, waypoint));
    }

    this->m_scroller->m_contentLayer->updateLayout();
}

CCNode* MainMenuPopup::ResolveWaypointUIRoot(CCNode* current) {
    CCNode* row = nullptr;
    for (; current; current = current->getParent()) {
        if (current->getID() == GENERIC_WAYPOINT_ROW_NAME) {
            row = current;
            break;
        }
    }

    return row;
}

void MainMenuPopup::onRowGlobalToggleClicked(CCObject* sender) {
    auto* node = static_cast<CCNode*>(sender);

    CCNode* row = this->ResolveWaypointUIRoot(node);
    if (!row) {
        log::warn("could not find waypoint root CCNode");
        return;
    }

    auto weak = this->GetAssociatedWaypoint(row);
    auto waypoint = weak? weak->lock(): nullptr;

    if (!waypoint) {
        log::warn("could not find associated waypoint for UI row");
        return;
    }

    auto* DataManager = DataManager::GetSingleton();
    DataManager->ToggleWaypoint(waypoint);
}

void MainMenuPopup::onRowEnabledToggleClicked(CCObject* sender) {
    auto* node = static_cast<CCNode*>(sender);

    CCNode* row = this->ResolveWaypointUIRoot(node);
    if (!row) {
        log::warn("could not find waypoint root CCNode");
        return;
    }

    auto weak = this->GetAssociatedWaypoint(row);
    auto waypoint = weak? weak->lock(): nullptr;

    if (!waypoint) {
        log::warn("could not find associated waypoint for UI row");
        return;
    }


    auto toggler = static_cast<CCMenuItemToggler*>(row->getChildByIDRecursive(GENERIC_WAYPOINT_ENABLED_TOGGLE_NAME));
    bool enabled = !toggler->m_toggled;
    log::debug("toggled: {}", enabled);
    waypoint->SetEnabled(enabled);


    if (waypoint->IsGlobal())
        DataManager::GetSingleton()->SaveGlobalWaypointInformation();
    else
        DataManager::GetSingleton()->SaveLevelWaypointInformation();
}

void MainMenuPopup::onRowDeleteButtonClicked(CCObject* sender) {
    auto* node = static_cast<CCNode*>(sender);

    CCNode* row = this->ResolveWaypointUIRoot(node);
    if (!row) {
        log::warn("could not find waypoint root CCNode");
        return;
    }

    auto weak = this->GetAssociatedWaypoint(row);
    auto waypoint = weak? weak->lock(): nullptr;

    if (!waypoint) {
        log::warn("could not find associated waypoint for UI row");
        return;
    }

    //now we prompt
    geode::createQuickPopup(
        "Confirm",
        "Would you like to delete this waypoint? <cy>This cannot be undone!</c>",
        "No", "Yes",
        [this, row, waypoint](auto, bool didClickYes) -> void {
            if (!didClickYes) return;

            DataManager::GetSingleton()->DeleteWaypoint(waypoint);
            row->removeFromParent();

            const auto it = this->m_waypointUIMap.find(row);
            this->m_waypointUIMap.erase(it);

            this->m_scroller->m_contentLayer->updateLayout();
            this->m_scroller->scrollToTop();
        }
    );
}


void MainMenuPopup::onDisableAllWaypointsButtonClicked(CCObject*) {
    geode::createQuickPopup(
        "Confirm",
        "Are you sure you want to disable <cy>all waypoints</c>?",
        "No", "Yes",
        [this](auto, bool didClickYes) -> void {
            if (!didClickYes) return;

            for (const auto& [ui, weakWaypoint]: this->m_waypointUIMap) {
                if (auto waypoint = weakWaypoint.lock()) {
                    waypoint->SetEnabled(false);

                    auto toggle = static_cast<CCMenuItemToggler*>(ui->getChildByIDRecursive(GENERIC_WAYPOINT_ENABLED_TOGGLE_NAME));
                    toggle->toggle(false);
                }
            }

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
            DataManager::GetSingleton()->DeleteAllWaypoints();

            for (const auto& [ui, _wp]: this->m_waypointUIMap)
                ui->removeFromParent();

            this->m_waypointUIMap.clear();
            this->m_scroller->m_contentLayer->updateLayout();
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