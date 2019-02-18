#include <random>

#include "core/Percept.hpp"
#include "BasicVacuumEnvironment.hpp"
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

BasicVacuumEnvironment::BasicVacuumEnvironment( unsigned int x, unsigned int y )
        : x( x ), y( y ), locations( x, y ) {
    using std::bind;
    using std::mt19937;
    using std::uniform_int_distribution;
    std::random_device rd{};
    static auto        randomBool = bind( uniform_int_distribution( 0, 1 ), // NOLINT(cert-err58-cpp)
                                          mt19937( rd()));

    for ( unsigned int i = 0; i < x; ++i )
        for ( unsigned int j = 0; j < y; ++j )
            locations( i, j ) = randomBool() ? LocationState::CLEAN : LocationState::DIRTY;

    genXCoord = bind( uniform_int_distribution<>( 0, x - 1 ), mt19937( rd()));
    genYCoord = bind( uniform_int_distribution<>( 0, y - 1 ), mt19937( rd()));
}

std::unique_ptr<Percept> BasicVacuumEnvironment::getPerceptSeenBy( const Agent& agent ) {
    auto location = getAgentLocation( agent );
    auto state    = getLocationState( location );
    auto p        = new LocalVacuumEnvironmentPercept( location, state );
    return std::unique_ptr<Percept>( p );
}

Location BasicVacuumEnvironment::getAgentLocation( const Agent& agent ) const {
    return agentLocations.at( agent );
}

void BasicVacuumEnvironment::setAgentLocation( const Agent& agent, Location location ) {
    agentLocations[ agent ] = std::move( location );
}

Location& BasicVacuumEnvironment::getAgentLocationByRef( const Agent& agent ) {
    return agentLocations[ agent ];
}

void BasicVacuumEnvironment::removeAgent( const Agent& agent ) {
    agentLocations.erase( agent );
    Environment::removeAgent( agent );
}

bool BasicVacuumEnvironment::isDone() const {
    return isDone_ || getStepCount() >= maxSteps || Environment::isDone();
}

const BasicVacuumEnvironment::Locations& BasicVacuumEnvironment::getLocations() const {
    return locations;
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
    if ( isNew ) agentLocations[ agent ] = Location( { genXCoord(), genYCoord() } );
    return isNew;
}

void BasicVacuumEnvironment::executeAction( const Agent& agent, const Action& action ) {
    auto& location = getAgentLocationByRef( agent );

    if ( action == ACTION_MOVE_DOWN ) {
        if ( location.y < y - 1 ) {
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
        if ( location.x < x - 1 ) {
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
    else if ( action == ACTION_SUCK ) { locations( location.x, location.y ) = LocationState::CLEAN; }
    else if ( action == Action::noOp()) { isDone_ = true; }
    else throw std::out_of_range( "Unrecognized action received" );
}


void BasicVacuumEnvironment::createExogenousChange() {
    int amount = isDone() ? maxSteps - stepCount : 1;
    ++stepCount;
    for ( unsigned i = 0; i < x; ++i )
        for ( unsigned j = 0; j < y; ++j )
            if ( locations( i, j ) == LocationState::CLEAN )
                for ( Agent& agent: getAgents())
                    updatePerformanceMeasure( agent, amount );
}

