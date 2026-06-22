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
            WaypointBehaviourType m_behaviourType;
            float m_activationPercentage; //TODO: check gd actually uses a float or other floating-point thing for this umm can change if not sob
            bool m_bEnabled;
            bool m_bIsGlobal;

        public:
            explicit Waypoint(WaypointBehaviourType type, float percentage = 0):
                m_behaviourType(type), m_activationPercentage(percentage), m_bEnabled(true), m_bIsGlobal(false) {};
            ~Waypoint() = default;
            Waypoint(const Waypoint&) = delete;
            Waypoint& operator=(const Waypoint&) = delete;
            Waypoint(Waypoint&&) = delete;



            void SetTriggerPercentage(float newPercentage);
            uint8_t GetTriggerPercentage() const {return this->m_activationPercentage;}

            bool IsEnabled() const {return this->m_bEnabled;}
            void SetEnabled(bool newEnabled);

            bool IsGlobal() const {return this->m_bIsGlobal;}
            void SetGlobal(bool newGlobal);

            bool ShouldPause(float currentPercentage) const;
            bool operator==(const Waypoint& other) const;
    };
}
