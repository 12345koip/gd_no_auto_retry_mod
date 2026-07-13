#pragma once

namespace AutoPauseMod::DataManagement {
    class DataManager;

    namespace DataPersistence {
        struct WaypointInformation;
    }
}

namespace AutoPauseMod::Waypoints {
    enum class WaypointBehaviourType: uint8_t {
        FromStartPosOnly, //e.g. a waypoint of 53% and a startpos of 40%: would trigger at 93%
        OnExactPercentage, //if you die exactly on a given percentage, no later, no earlier
        FromStartOnly, //any percentage from 0. ignores start position
        FromAnywhere //if you pass a specific percentage in a level regardless of startpos
    };

    //scary maths...!
    //jk, this is far superior to a switch-case...
    inline WaypointBehaviourType GetNextWaypointType(const WaypointBehaviourType currentType) {
        return static_cast<WaypointBehaviourType>(
            (static_cast<uint8_t>(currentType) + 1) % 4
        );
    }

    inline WaypointBehaviourType GetPreviousWaypointType(const WaypointBehaviourType currentType) {
        return static_cast<WaypointBehaviourType>(
            (static_cast<uint8_t>(currentType) + 3) % 4
        );
    }

    inline const char* WaypointTypeToString(const WaypointBehaviourType type) {
        switch (type) {
            case WaypointBehaviourType::FromAnywhere:
                return "ANY";
            case WaypointBehaviourType::FromStartOnly:
                return "STA";
            case WaypointBehaviourType::FromStartPosOnly:
                return "SP";
            case WaypointBehaviourType::OnExactPercentage:
                return "ONP";


            default:
                return "UNK";
        }
    }



    class Waypoint final {
        private:
            float m_activationPercentage;
            WaypointBehaviourType m_behaviourType;
            bool m_bEnabled;
            bool m_bIsGlobal;


        public:
            explicit Waypoint(WaypointBehaviourType type, float percentage = 0):
                   m_behaviourType(type), m_activationPercentage(percentage), m_bEnabled(true), m_bIsGlobal(false) {};

            void SetBehaviourType(WaypointBehaviourType newType);
            WaypointBehaviourType GetBehaviourType() const {return this->m_behaviourType;}

            void SetTriggerPercentage(float newPercentage);
            float GetTriggerPercentage() const {return this->m_activationPercentage;}

            void SetEnabled(bool newEnabled);
            bool IsEnabled() const {return this->m_bEnabled;}

            bool IsGlobal() const {return this->m_bIsGlobal;}
            void SetGlobal(bool newGlobal);

            bool ShouldPause(const DataManagement::DataManager* dataManager, const float currentPercentage) const;
    };
}
