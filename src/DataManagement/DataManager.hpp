//
// Created by katie on 22/06/2026.
//

#pragma once
#include <Geode/Geode.hpp>
#include <memory>
#include <vector>
#include "../Waypoints/Definitions.hpp"

namespace AutoPauseMod::DataManagement {
    class DataManager final {
        private:
            std::vector<std::shared_ptr<Waypoints::Waypoint>> m_loadedWaypoints {};
            float m_attemptStartPos = 0;

            DataManager() = default;
            ~DataManager() = default;
        public:
            static DataManager& GetSingleton() {
                static DataManager dataManager;
                return dataManager;
            }

            std::shared_ptr<Waypoints::Waypoint> NewWaypoint();

            [[nodiscard]] float GetAttemptStartPercentage() const {return this->m_attemptStartPos;}
    };
}
