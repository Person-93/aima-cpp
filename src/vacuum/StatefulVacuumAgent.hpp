#pragma once

#include <memory>
#include <boost/numeric/ublas/matrix.hpp>
#include "core/Agent.hpp"

// IWYU pragma: no_include <boost/numeric/ublas/functional.hpp>
// IWYU pragma: no_include <boost/numeric/ublas/storage.hpp>
// IWYU pragma: no_include "core/Action.hpp"

namespace aima::core { class Percept; }

namespace aima::vacuum {
    class BasicVacuumEnvironment;

    class StatefulVacuumAgent : public core::Agent {
    public:
        StatefulVacuumAgent( unsigned long x, unsigned long y );

        /**
         * Takes a reference to the environment that the agent will operate in.
         * The reference is not saved, it is only used to get the environment's dimensions.
         * @param environment
         */
        explicit StatefulVacuumAgent( const vacuum::BasicVacuumEnvironment& environment );

        std::unique_ptr<Agent> clone() const override;

        std::unique_ptr<core::Action> execute( const core::Percept& percept ) override;

    protected:
        StatefulVacuumAgent( const StatefulVacuumAgent& ) = default;

    private:
        bool allClean();

        boost::numeric::ublas::matrix<bool> state;
    };
}
