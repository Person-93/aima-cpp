#pragma once

#include <boost/numeric/ublas/matrix.hpp>

#include "core/Agent.hpp"
#include "core/Action.hpp"

namespace aima::core {
    class Percept;
}

namespace aima::vacuum {
    class StatefulVacuumAgent : public core::Agent {
    public:
        StatefulVacuumAgent( unsigned long x, unsigned long y );

        const core::Action& execute( const core::Percept& percept ) override;

    private:
        bool allClean();

        boost::numeric::ublas::matrix<bool> state;
        unsigned long                       x;
        unsigned long                       y;
    };
}
