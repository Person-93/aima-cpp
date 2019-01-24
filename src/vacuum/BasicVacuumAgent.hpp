#pragma once

#include <core/Agent.hpp>

namespace aima::vacuum {
    namespace {
        using namespace aima::core;
    }
    class BasicVacuumAgent : public core::Agent {
    public:
        BasicVacuumAgent() = default;

        const Action& execute( const Percept& percept ) override;
    };
}
