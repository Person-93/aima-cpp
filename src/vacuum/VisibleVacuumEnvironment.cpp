#include "VisibleVacuumEnvironment.hpp"
#include "GlobalVacuumEnvironmentPercept.hpp"
#include "core/Action.hpp"

using namespace aima::core;
using namespace aima::vacuum;

std::unique_ptr<Percept> VisibleVacuumEnvironment::getPerceptSeenBy( const Agent& agent ) {
    return std::unique_ptr<Percept>{ new GlobalVacuumEnvironmentPercept{ getAgentLocation( agent ), getLocations() }};
}

void VisibleVacuumEnvironment::executeAction( const Agent& agent, const Action& action ) {
    if ( action == VisibleVacuumEnvironment::ACTION_PLANNING ) {
        updatePerformanceMeasure( agent, -.1 );
    }
    else
        BasicVacuumEnvironment::executeAction( agent, action );
}

const Action VisibleVacuumEnvironment::ACTION_PLANNING{ "Planning" }; // NOLINT(cert-err58-cpp)
