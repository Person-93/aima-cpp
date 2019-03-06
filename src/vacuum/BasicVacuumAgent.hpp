#pragma once

#include <core/Agent.hpp>
#include <core/Action.hpp>

namespace aima::core {
    class Percept;
}

namespace aima::vacuum {
    class BasicVacuumAgent : public core::Agent {
    public:
        BasicVacuumAgent() = default;

        const core::Action& execute( const core::Percept& percept ) override;
    };
}
