#pragma once
#include <Geode/Geode.hpp>

namespace AutoPauseMod::Waypoints {
    class Waypoint;
}

namespace AutoPauseMod::UI::WaypointUI {
    class WaypointRow final: public cocos2d::CCNode {
        private:
            CCMenuItemToggler* m_pGlobalToggle;
            CCMenuItemToggler* m_pEnabledToggle;
            geode::TextInput* m_pTextInputBox;
            std::weak_ptr<Waypoints::Waypoint> m_associatedWaypoint;
            geode::WeakRef<geode::ScrollLayer> m_ownerWaypointScroller;

            void onGlobalToggleClicked(CCObject*) const;
            void onDeleteButtonClicked(CCObject*);
            void onEnabledToggleClicked(CCObject*) const;

        public:
            bool init(const std::shared_ptr<Waypoints::Waypoint>& waypoint, const geode::WeakRef<geode::ScrollLayer>& scroller);

            static WaypointRow* create(const std::shared_ptr<Waypoints::Waypoint>& waypoint, const geode::WeakRef<geode::ScrollLayer>& scroller) {
                auto row = new WaypointRow();

                if (row->init(waypoint, scroller)) {
                    row->autorelease();
                    return row;
                }

                delete row;
                return nullptr;
            }

            std::shared_ptr<Waypoints::Waypoint> GetAssociatedWaypoint() const {
                return this->m_associatedWaypoint.lock();
            }

            void SetEnabled(bool newEnabled);
            void Delete(bool alsoDeleteWaypoint = false);
    };
}