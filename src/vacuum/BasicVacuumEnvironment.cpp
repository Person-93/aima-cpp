#include <random>
#include <stdexcept>
#include <utility>
#include <string_view>

#include "core/Percept.hpp"
#include "core/Action.hpp"
#include "BasicVacuumEnvironment.hpp" // IWYU pragma: associated
#include "LocalVacuumEnvironmentPercept.hpp"

using namespace aima::core;
using namespace aima::vacuum;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
const Action BasicVacuumEnvironment::ACTION_MOVE_LEFT  = Action( "Left" );
const Action BasicVacuumEnvironment::ACTION_MOVE_RIGHT = Action( "Right" );
const Action BasicVacuumEnvironment::ACTION_MOVE_UP    = Action( "Up" );
const Action BasicVacuumEnvironment::ACTION_MOVE_DOWN  = Action( "Down" );
const Action BasicVacuumEnvironment::ACTION_SUCK       = Action( "Suck" );
#pragma clang diagnostic pop

BasicVacuumEnvironment::BasicVacuumEnvironment( unsigned long x, unsigned long y )
        : locations( x, y ) {
    static auto randomBool = std::bind( std::uniform_int_distribution<uint_fast8_t>( 0, 1 ),
                                        std::mt19937( std::random_device()()));

    for ( unsigned int i = 0; i < x; ++i )
        for ( unsigned int j = 0; j < y; ++j )
            locations( i, j ) = randomBool() ? LocationState::CLEAN : LocationState::DIRTY;
}

std::unique_ptr<Percept> BasicVacuumEnvironment::getPerceptSeenBy( const Agent& agent ) {
    auto location = getAgentLocation( agent );
    auto state    = getLocationState( location );
    return std::make_unique<LocalVacuumEnvironmentPercept>( location, state );
}

void BasicVacuumEnvironment::removeAgent( const Agent& agent ) {
    agentLocations.erase( agent );
    Environment::removeAgent( agent );
}

bool BasicVacuumEnvironment::isDone() const {
    return agentStopped() || getStepCount() >= maxSteps() || Environment::isDone();
}

LocationState BasicVacuumEnvironment::getLocationState( Location location ) const {
    auto[x, y] = std::move( location );
    return locations( x, y );
}

void BasicVacuumEnvironment::setLocationState( Location location, LocationState state ) {
    auto[x, y] = std::move( location );
    locations( x, y ) = state;
}

bool BasicVacuumEnvironment::addAgent( Agent& agent ) {
    auto isNew = Environment::addAgent( agent );
    if ( isNew ) setAgentLocation( agent, randomLocation());
    return isNew;
}

void BasicVacuumEnvironment::executeAction( const Agent& agent, const Action& action ) {
    auto& location = getAgentLocationByRef( agent );

    if ( action == ACTION_MOVE_DOWN ) {
        if ( location.y < getY() - 1 ) {
            ++location.y;
            updatePerformanceMeasure( agent, -1 );
        }
    }
    else if ( action == ACTION_MOVE_UP ) {
        if ( location.y > 0 ) {
            --location.y;
            updatePerformanceMeasure( agent, -1 );
        }
    }
    else if ( action == ACTION_MOVE_RIGHT ) {
        if ( location.x < getX() - 1 ) {
            ++location.x;
            updatePerformanceMeasure( agent, -1 );
        }
    }
    else if ( action == ACTION_MOVE_LEFT ) {
        if ( location.x > 0 ) {
            --location.x;
            updatePerformanceMeasure( agent, -1 );
        }
    }
    else if ( action == ACTION_SUCK ) { setLocationState( location, LocationState::CLEAN ); }
    else if ( action == Action::noOp()) { agentStopped( true ); }
    else throw std::out_of_range( "Unrecognized action received" );
}

void BasicVacuumEnvironment::createExogenousChange() {
    int amount = isDone() ? maxSteps() - stepCount : 1;
    ++stepCount;
    for ( unsigned i = 0; i < getX(); ++i )
        for ( unsigned j = 0; j < getY(); ++j )
            if ( locations( i, j ) == LocationState::CLEAN )
                for ( Agent& agent: getAgents())
                    updatePerformanceMeasure( agent, amount );
}

Location BasicVacuumEnvironment::randomLocation() const {
    static std::mt19937 engine(( std::random_device()()));
    return Location{ std::uniform_int_distribution<unsigned>( 0, getX() - 1 )( engine ),
                     std::uniform_int_distribution<unsigned>( 0, getY() - 1 )( engine ) };
}
