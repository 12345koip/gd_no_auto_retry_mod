#include "WaypointRow.hpp"
#include "../Utils.hpp"
#include "../Selectors/WaypointBehaviourSelector.hpp"
#include "../../DataManagement/DataManager.hpp"

using namespace AutoPauseMod::DataManagement;
using namespace AutoPauseMod::UI::WaypointUI;
using namespace geode::prelude;

//...
//ok thats a long signature lets never do that again
bool WaypointRow::init(const std::shared_ptr<Waypoints::Waypoint>& waypoint, const geode::WeakRef<ScrollLayer>& scroller, const geode::WeakRef<Main::MainMenuPopup>& mainPopup) {
    if (!CCNode::init())
        return false;

    this->m_associatedWaypoint = waypoint;
    this->m_ownerWaypointScroller = scroller;
    this->m_ownerPopup = mainPopup;

    //root row
    this->setContentSize({270.0f, 34.0f});
    this->setLayoutOptions(AxisLayoutOptions::create()->setLength(34.0f));
    this->setAnchorPoint({0.5f, 0.5f});
    this->setID("WaypointRow"_spr);
    this->setZOrder(2);

    auto bg = CCScale9Sprite::create("square02b_small.png");
    bg->setContentSize(this->getContentSize());
    bg->setAnchorPoint({0.5f, 0.5f});
    bg->setPosition(this->getContentSize() / 2);
    bg->setColor({245, 167, 105});
    this->addChild(bg);


    //labels
    auto label_enabled = Utils::MakeLabel(
        "Enabled",
        "bigFont.fnt",
        {0.f, 0.5f},
        {57.f, 17.f},
        0.4f
    );
    this->addChild(label_enabled);

    auto label_global = Utils::MakeLabel(
        "Global",
        "bigFont.fnt",
        {0.f, 0.5f},
        {132.f, 17.f},
        0.4f
    );
    this->addChild(label_global);

    auto label_behaviour = Utils::MakeLabel(
        "From",
        "bigFont.fnt",
        {0.5f, 0.5f},
        {222.f, 28.f},
        0.3f
    );
    this->addChild(label_behaviour);



    //menu + togglers
    auto menu = CCMenu::create();
    menu->setPosition({135.0f, 0.0f});
    menu->setID("ToggleMenu"_spr);
    this->addChild(menu);

    this->m_pEnabledToggle = Utils::MakeToggle(
        this,
        menu_selector(WaypointRow::onEnabledToggleClicked),
        0.65f,
        {-15.0f, 17.0f},
        waypoint->IsEnabled()
    );
    menu->addChild(this->m_pEnabledToggle);


    this->m_pGlobalToggle = Utils::MakeToggle(
        this,
        menu_selector(WaypointRow::onGlobalToggleClicked),
        0.65f,
        {53.0f, 17.0f},
        waypoint->IsGlobal()
    );
    menu->addChild(this->m_pGlobalToggle);

    //delete button.
    auto button_delete = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png"),
        this,
        menu_selector(WaypointRow::onDeleteButtonClicked)
    );
    button_delete->m_baseScale = 0.4f;
    button_delete->setScale(0.4f);
    button_delete->setPosition({124.0f, 17.0f});
    menu->addChild(button_delete);


    //input bits
    this->m_pTextInputBox = TextInput::create(55.0f, "");
    this->m_pTextInputBox->setPosition({22.0f, 17.0f});
    this->m_pTextInputBox->setScale(0.7f);
    this->m_pTextInputBox->setAnchorPoint({0.5f, 0.5f});
    this->m_pTextInputBox->setFilter("0123456789");
    this->m_pTextInputBox->setString(
        std::to_string(static_cast<int>(waypoint->GetTriggerPercentage())),
        false
    );
    this->addChild(m_pTextInputBox);

    auto label_percentage = Utils::MakeLabel(
        "%",
        "bigFont.fnt",
        {0.0f, 0.5f},
        {42.0f, 17.0f},
        0.4f
    );
    this->addChild(label_percentage);


    //behaviour
    auto selector = Selectors::WaypointBehaviourSelector::create();
    selector->setPosition({222.f, 13.f});
    selector->setScale(.5f);
    selector->setState(waypoint->GetBehaviourType());
    this->addChild(selector);


    //and finally callbacks
    selector->setCallback(
        [weakWp = this->m_associatedWaypoint](const Waypoints::WaypointBehaviourType newBehaviour) -> void {
            if (auto waypoint = weakWp.lock()) {
                waypoint->SetBehaviourType(newBehaviour);

                if (waypoint->IsGlobal())
                    DataManager::GetSingleton()->SaveGlobalWaypointInformation();
                else
                    DataManager::GetSingleton()->SaveLevelWaypointInformation();
            }
        }
    );

    this->m_pTextInputBox->setCallback(
        [this](const std::string& newText) -> void {
            auto result = geode::utils::numFromString<int>(newText);
            int value = result? result.unwrap(): 0;
            value = std::clamp(value, 0, 100);

            //input value was invalid, update text to reflect used value
            auto valueStr = std::to_string(value);
            if (newText != valueStr)
                this->m_pTextInputBox->setString(valueStr, false);

            if (auto waypoint = this->m_associatedWaypoint.lock()) {
                waypoint->SetTriggerPercentage(value);
                DataManager::GetSingleton()->UpdateWaypointListPosition(waypoint);
            }
        }
    );

    return true;
}

void WaypointRow::setEnabled(bool newEnabled) {
    if (this->m_associatedWaypoint.expired()) {
        log::warn("could not set waypoint enabled; waypoint expired");
        return;
    }

    auto waypoint = this->m_associatedWaypoint.lock();
    waypoint->SetEnabled(newEnabled);

    this->m_pEnabledToggle->toggle(newEnabled);
}

void WaypointRow::Delete(bool alsoDeleteWaypoint) {
    auto waypoint = this->m_associatedWaypoint.lock();
    this->removeFromParent();

    if (alsoDeleteWaypoint && waypoint)
        DataManager::GetSingleton()->DeleteWaypoint(waypoint);
}




//listeners

void WaypointRow::onDeleteButtonClicked(CCObject*) {
    if (this->m_associatedWaypoint.expired()) {
        log::warn("associated waypoint for row expired");
        return;
    }

    geode::createQuickPopup(
        "Confirm",
        "Would you like to delete this waypoint? <cy>This cannot be undone!</c>",
        "No", "Yes",
        [this](auto, bool didClickYes) -> void {
            if (!didClickYes) return;
            auto scroller = this->m_ownerWaypointScroller.lock();
            this->Delete(true);

            if (scroller) {
                scroller->m_contentLayer->updateLayout();
                scroller->scrollToTop();
            }
        }
    );
}

void WaypointRow::onEnabledToggleClicked(CCObject*) const {
    if (this->m_associatedWaypoint.expired()) {
        log::warn("associated waypoint for row expired");
        return;
    }

    auto waypoint = this->m_associatedWaypoint.lock();
    waypoint->SetEnabled(!waypoint->IsEnabled());

    if (waypoint->IsGlobal())
        DataManager::GetSingleton()->SaveGlobalWaypointInformation();
    else
        DataManager::GetSingleton()->SaveLevelWaypointInformation();
}

void WaypointRow::onGlobalToggleClicked(CCObject*) const {
    if (this->m_associatedWaypoint.expired()) {
        log::warn("associated waypoint for row expired");
        return;
    }

    auto waypoint = this->m_associatedWaypoint.lock();
    DataManager::GetSingleton()->ToggleWaypoint(waypoint);
}
