#pragma once

#include "BasicVacuumEnvironment.hpp"

namespace aima::vacuum {
    class VisibleVacuumEnvironment : public BasicVacuumEnvironment {
    public:
        using BasicVacuumEnvironment::BasicVacuumEnvironment;

        std::unique_ptr<core::Percept> getPerceptSeenBy( const core::Agent& agent ) override;

        void executeAction( const core::Agent& agent, const core::Action& action ) override;

        static const core::Action ACTION_PLANNING;
    };
}
