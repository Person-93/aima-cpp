#include "LocalVacuumEnvironmentPercept.hpp"

aima::vacuum::LocalVacuumEnvironmentPercept::LocalVacuumEnvironmentPercept( aima::vacuum::Location location,
                                                                aima::vacuum::LocationState locationState ) :
        agentLocation( location ), agentLocationState( locationState ) {}

void aima::vacuum::LocalVacuumEnvironmentPercept::print( std::ostream& out ) const {
    out << "AgentLocation=" << agentLocation << ", State=" << agentLocationState;
}
