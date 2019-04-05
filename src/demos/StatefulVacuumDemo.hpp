#pragma once

#include <iosfwd>
#include "core/Demo.hpp"
#include "../vacuum/BasicVacuumEnvironment.hpp"
#include "vacuum/StatefulVacuumAgent.hpp"

namespace aima::demos {
    class StatefulVacuumDemo : public core::Demo {
    public:
        explicit StatefulVacuumDemo( std::ostream& out );

    protected:
        core::Environment& environment() noexcept override;

        void init() override;

    private:
        vacuum::BasicVacuumEnvironment environment_;
        vacuum::StatefulVacuumAgent    agent;
    };
}
