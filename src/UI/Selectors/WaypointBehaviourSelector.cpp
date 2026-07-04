//
// Created by katie on 04/07/2026.
//

#include "WaypointBehaviourSelector.hpp"
#include <Geode/Geode.hpp>
#include <algorithm>

using namespace geode::prelude;
using namespace AutoPauseMod::UI::Selectors;

void WaypointBehaviourSelector::onLeftButtonClicked(CCObject*) {
    this->m_index = Waypoints::GetPreviousWaypointType(this->m_index);
    this->m_label->setString(Waypoints::WaypointTypeToString(this->m_index));

    if (this->m_callback)
        this->m_callback(this->m_index);
}

void WaypointBehaviourSelector::onRightButtonClicked(CCObject*) {
    this->m_index = Waypoints::GetNextWaypointType(this->m_index);
    this->m_label->setString(Waypoints::WaypointTypeToString(this->m_index));

    if (this->m_callback)
        this->m_callback(this->m_index);
}

bool WaypointBehaviourSelector::init() {
    if (!CCNode::init()) return false;

    auto menu = CCMenu::create();
    menu->setPosition(CCPointZero);
    this->addChild(menu);

    auto left = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png"),
        this,
        menu_selector(WaypointBehaviourSelector::onLeftButtonClicked)
    );

    auto right = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png"),
        this,
        menu_selector(WaypointBehaviourSelector::onRightButtonClicked)
    );

    this->m_label = CCLabelBMFont::create(
        Waypoints::WaypointTypeToString(this->m_index),
        "bigFont.fnt"
    );

    left->setPosition({-40.f, 0.f});
    this->m_label->setPosition({0.f, 0.f});
    right->setPosition({40.f, 0.f});

    menu->addChild(left);
    menu->addChild(this->m_label);
    menu->addChild(right);
    this->m_menu = menu;

    return true;
}

void WaypointBehaviourSelector::setScale(float scale) {
    CCNode::setScale(scale);
    this->m_label->setScale(std::clamp(scale + 0.3f, 0.0f, 1.0f));
}

void WaypointBehaviourSelector::setCallback(const std::function<void(const Waypoints::WaypointBehaviourType)>& callback) {
    this->m_callback = callback;
}
