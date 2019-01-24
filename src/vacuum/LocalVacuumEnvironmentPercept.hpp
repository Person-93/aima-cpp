#pragma once

#include <core/Percept.hpp>

#include "types.hpp"

namespace aima::vacuum {
    class LocalVacuumEnvironmentPercept : public core::Percept {
    public:
        LocalVacuumEnvironmentPercept( Location location, LocationState locationState );

        const Location      agentLocation;
        const LocationState agentLocationState;

    protected:
        void print( std::ostream& out ) const override;
    };
}
