//
// Created by katie on 22/06/2026.
//

#pragma once
#include <cstdint>

namespace AutoPauseMod::DataManagement {
    class DataManager;

    namespace DataPersistence {
        struct WaypointInformation;
    }
}

namespace AutoPauseMod::Waypoints {
    enum class WaypointBehaviourType: uint8_t {
        FromStartPosOnly, //e.g. a waypoint of 53% and a startpos of 40%: would trigger at 93%
        FromStartOnly,    //any percentage from 0. ignores start position
        FromAnywhere      //if you pass a specific percentage in a level regardless of startpos
    };

    inline WaypointBehaviourType GetNextWaypointType(const WaypointBehaviourType currentType) {
        if (currentType == WaypointBehaviourType::FromAnywhere)
            return WaypointBehaviourType::FromStartPosOnly;

        return static_cast<WaypointBehaviourType>(
            static_cast<uint8_t>(currentType) + 1
        );
    }

    class Waypoint final {
        private:
            int m_levelID;
            float m_activationPercentage;
            WaypointBehaviourType m_behaviourType;
            bool m_bEnabled;
            bool m_bIsGlobal;

        public:
            ~Waypoint() = default;
            Waypoint(const Waypoint&) = delete;
            Waypoint& operator=(const Waypoint&) = delete;
            Waypoint(Waypoint&&) = delete;

            explicit Waypoint(WaypointBehaviourType type, float percentage = 0, uint32_t levelID = 0):
                   m_behaviourType(type), m_activationPercentage(percentage), m_bEnabled(true), m_bIsGlobal(false), m_levelID(levelID) {};

            static std::shared_ptr<Waypoint> FromWaypointInformation(const int levelId, const DataManagement::DataPersistence::WaypointInformation& info);
            static std::shared_ptr<Waypoint> FromWaypointInformation(const DataManagement::DataPersistence::WaypointInformation& info);

            void SetBehaviourType(WaypointBehaviourType newType);
            [[nodiscard]] WaypointBehaviourType GetBehaviourType() const {return this->m_behaviourType;}

            void SetLevelID(int newLevelId);
            [[nodiscard]] int GetLevelID() const {return this->m_levelID;}

            void SetTriggerPercentage(float newPercentage);
            [[nodiscard]] float GetTriggerPercentage() const {return this->m_activationPercentage;}

            void SetEnabled(bool newEnabled);
            [[nodiscard]] bool IsEnabled() const {return this->m_bEnabled;}

            void SetGlobal(bool newGlobal);
            [[nodiscard]] bool IsGlobal() const {return this->m_bIsGlobal;}

            bool ShouldPause(const DataManagement::DataManager* dataManager, float currentPercentage) const;
            bool operator==(const std::shared_ptr<Waypoint> other) const;
    };
}
