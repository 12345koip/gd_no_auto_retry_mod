#pragma once
#include "../../Waypoints/Definitions.hpp"

namespace AutoPauseMod::UI::Selectors {
    class WaypointBehaviourSelector final: public cocos2d::CCNode {
        private:
            cocos2d::CCMenu* m_menu = nullptr;
            cocos2d::CCLabelBMFont* m_label = nullptr;

            geode::Function<void(const Waypoints::WaypointBehaviourType)> m_callback = nullptr;
            Waypoints::WaypointBehaviourType m_index = Waypoints::WaypointBehaviourType::FromAnywhere;

            void onLeftButtonClicked(cocos2d::CCObject*);
            void onRightButtonClicked(cocos2d::CCObject*);

        public:
            bool init() override;
            static WaypointBehaviourSelector* create() {
                auto selector = new WaypointBehaviourSelector();

                if (selector->init()) {
                    selector->autorelease();
                    return selector;
                }

                delete selector;
                return nullptr;
            }

            void setState(const Waypoints::WaypointBehaviourType newState);
            void setScale(float scale) override;
            void setCallback(geode::Function<void(const Waypoints::WaypointBehaviourType)>&& callback);
            Waypoints::WaypointBehaviourType GetSelectedBehaviourType() const {return this->m_index;};
    };
}
