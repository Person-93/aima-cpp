#include <utility>
#include <ostream>

#include "LocalVacuumEnvironmentPercept.hpp" // IWYU pragma: associated

// IWYU pragma: no_include "util/PrintableMixin.hpp"

using namespace aima::vacuum;

LocalVacuumEnvironmentPercept::LocalVacuumEnvironmentPercept( Location location, LocationState locationState ) :
        agentLocation( std::move( location )), agentLocationState( locationState ) {}

void LocalVacuumEnvironmentPercept::print( std::ostream& out ) const {
    out << "AgentLocation=" << agentLocation << ", State=" << agentLocationState;
}
