#pragma once

#include <memory>
#include "core/Agent.hpp" // IWYU pragma: export

// IWYU pragma: no_include "core/Action.hpp"

namespace aima::core { class Percept; }

namespace aima::vacuum {
    class BasicVacuumAgent : public core::Agent {
    public:
        BasicVacuumAgent();

        std::unique_ptr<Agent> clone() const override;

        const core::Action& execute( const core::Percept& percept ) override;
    };
}
