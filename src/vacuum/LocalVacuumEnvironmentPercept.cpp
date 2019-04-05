#include "LocalVacuumEnvironmentPercept.hpp"
#include <ostream>  // IWYU pragma: keep
#include "util/define_logger.hpp"


using namespace aima::vacuum;

DEFINE_LOGGER( LocalVacuumEnvironmentPercept );

LocalVacuumEnvironmentPercept::LocalVacuumEnvironmentPercept( const Location& location, LocationState locationState ) :
        agentLocation( location ), agentLocationState( locationState ) { TRACE; }

void LocalVacuumEnvironmentPercept::print( std::ostream& out ) const {
    out << "AgentLocation=" << agentLocation << ", State=" << agentLocationState;
}

std::unique_ptr<aima::core::Percept> LocalVacuumEnvironmentPercept::clone() const {
    TRACE;
    return std::unique_ptr<Percept>( new LocalVacuumEnvironmentPercept( *this ));
}
