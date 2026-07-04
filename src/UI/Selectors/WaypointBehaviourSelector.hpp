//
// Created by katie on 04/07/2026.
//

#pragma once
#include "../../Waypoints/Definitions.hpp"
#include <functional>

namespace AutoPauseMod::UI::Selectors {
    class WaypointBehaviourSelector final: public cocos2d::CCNode {
        private:
            cocos2d::CCMenu* m_menu = nullptr;
            cocos2d::CCLabelBMFont* m_label = nullptr;

            std::function<void(const Waypoints::WaypointBehaviourType)> m_callback = nullptr;
            Waypoints::WaypointBehaviourType m_index = Waypoints::WaypointBehaviourType::FromAnywhere;

            void onLeftButtonClicked(cocos2d::CCObject*);
            void onRightButtonClicked(cocos2d::CCObject*);

            bool init() override;

        public:
            static WaypointBehaviourSelector* create() {
                auto selector = new WaypointBehaviourSelector();

                if (selector->init()) {
                    selector->autorelease();
                    return selector;
                }

                delete selector;
                return nullptr;
            }

            void setScale(float scale) override;
            void setCallback(const std::function<void(const Waypoints::WaypointBehaviourType)>& callback);
            Waypoints::WaypointBehaviourType GetSelectedBehaviourType() const {return this->m_index;};
    };
}
