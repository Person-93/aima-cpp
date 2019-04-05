#pragma once

#include "core/Demo.hpp" // IWYU pragma: associated
#include "../vacuum/BasicVacuumEnvironment.hpp"
#include "vacuum/BasicVacuumAgent.hpp"


namespace aima::demos {
    class TrivialVacuumDemo : public core::Demo {
    public:
        explicit TrivialVacuumDemo( std::ostream& out );

    protected:
        core::Environment& environment() noexcept override;

        void init() override;

    private:
        vacuum::BasicVacuumEnvironment environment_;
        vacuum::BasicVacuumAgent       agent;
    };
}
