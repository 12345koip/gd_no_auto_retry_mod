//
// Created by katie on 22/06/2026.
//

#pragma once
#include <cstdint>

namespace AutoPauseMod::Waypoints {
    enum class WaypointBehaviourType: uint8_t {
        FromStartPosOnly, //e.g. a waypoint of 53% and a startpos of 40%: would trigger at 93%
        FromStartOnly,    //any percentage from 0. ignores start position
        FromAnywhere      //if you pass a specific percentage in a level regardless of startpos
    };

    class Waypoint final {
        private:
            uint64_t m_levelID; //TODO: check it's not uint32_t
            WaypointBehaviourType m_behaviourType;
            float m_activationPercentage; //TODO: check gd actually uses a float or other floating-point thing for this umm can change if not sob
            bool m_bEnabled;
            bool m_bIsGlobal;


        public:
            explicit Waypoint(WaypointBehaviourType type, float percentage = 0, uint32_t levelID = 0):
                m_behaviourType(type), m_activationPercentage(percentage), m_bEnabled(true), m_bIsGlobal(false), m_levelID(levelID) {
                    //if the given level ID is 0 and isGlobal is false, we will quite simply
                    //just assume the level ID could not be found or else a problem arose
                    //and just "lock" it as global for safety.
                    if (levelID == 0 && !this->m_bIsGlobal)
                        this->m_bIsGlobal = true;
                };
            ~Waypoint() = default;
            Waypoint(const Waypoint&) = delete;
            Waypoint& operator=(const Waypoint&) = delete;
            Waypoint(Waypoint&&) = delete;


            void SetBehaviourType(WaypointBehaviourType newType);
            WaypointBehaviourType GetBehaviourType() const {return this->m_behaviourType;}

            void SetLevelID(uint64_t newLevelId);
            uint64_t GetLevelID() const {return this->m_levelID;}

            void SetTriggerPercentage(float newPercentage);
            uint8_t GetTriggerPercentage() const {return this->m_activationPercentage;}

            bool IsEnabled() const {return this->m_bEnabled;}
            void SetEnabled(bool newEnabled);

            bool IsGlobal() const {return this->m_bIsGlobal;}
            void SetGlobal(bool newGlobal);

            bool ShouldPause(float currentPercentage) const;
            bool operator==(const std::shared_ptr<Waypoint> other) const;
    };
}
