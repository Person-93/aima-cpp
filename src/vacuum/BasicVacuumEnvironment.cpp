#include <random>

#include <core/Percept.hpp>

#include "BasicVacuumEnvironment.hpp"
#include "LocalVacuumEnvironmentPercept.hpp"

using namespace aima::core;

namespace aima::vacuum {
    std::unique_ptr<Percept> BasicVacuumEnvironmentBase::getPerceptSeenBy( const Agent& agent ) {
        auto location = getAgentLocation( agent );
        auto state    = getLocationState( location );
        auto p        = new LocalVacuumEnvironmentPercept( location, state );
        return std::unique_ptr<Percept>( p );
    }

    Location BasicVacuumEnvironmentBase::getAgentLocation( const Agent& agent ) const {
        return agentLocations.at( agent );
    }

    void BasicVacuumEnvironmentBase::setAgentLocation( const Agent& agent, Location location ) {
        agentLocations[ agent ] = location;
    }

    Location& BasicVacuumEnvironmentBase::getAgentLocationByRef( const Agent& agent ) {
        return agentLocations[ agent ];
    }

    void BasicVacuumEnvironmentBase::removeAgent( const Agent& agent ) {
        agentLocations.erase( agent );
        Environment::removeAgent( agent );
    }

    bool BasicVacuumEnvironmentBase::isDone() const {
        return isDone_ || getStepCount() >= maxSteps || Environment::isDone();
    }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
    const Action BasicVacuumEnvironmentBase::ACTION_MOVE_LEFT  = Action( "Left" );
    const Action BasicVacuumEnvironmentBase::ACTION_MOVE_RIGHT = Action( "Right" );
    const Action BasicVacuumEnvironmentBase::ACTION_MOVE_UP    = Action( "Up" );
    const Action BasicVacuumEnvironmentBase::ACTION_MOVE_DOWN  = Action( "Down" );
    const Action BasicVacuumEnvironmentBase::ACTION_SUCK       = Action( "Suck" );
#pragma clang diagnostic pop
}
