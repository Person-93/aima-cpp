#include "BasicVacuumEnvironment.hpp"
#include <random>
#include <cstdint>
#include <algorithm>
#include "core/Action.hpp"
#include "core/Exception.hpp"
#include "LocalVacuumEnvironmentPercept.hpp"
#include "util/parseTitle.hpp"
#include "util/define_logger.hpp"


using namespace aima::core;
using namespace aima::vacuum;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
const Action BasicVacuumEnvironment::ACTION_MOVE_LEFT( "Left" );
const Action BasicVacuumEnvironment::ACTION_MOVE_RIGHT( "Right" );
const Action BasicVacuumEnvironment::ACTION_MOVE_UP( "Up" );
const Action BasicVacuumEnvironment::ACTION_MOVE_DOWN( "Down" );
const Action BasicVacuumEnvironment::ACTION_SUCK( "Suck" );
#pragma clang diagnostic pop

DEFINE_LOGGER( BasicVacuumEnvironment );

namespace {
    std::mt19937& randomnessEngine() {
        static std::mt19937 engine( std::random_device{}());
        return engine;
    }
}

BasicVacuumEnvironment::BasicVacuumEnvironment( unsigned long x, unsigned long y )
        : locations( x, y ) {
    TRACE;
    LOG4CPLUS_INFO( GetLogger(), "Creating environment. id: " << id());

    static auto randomBool = []() -> bool {
        static std::uniform_int_distribution<uint_fast8_t> distribution( 0, 1 );
        return distribution( randomnessEngine());
    };

    for ( unsigned int i = 0; i < x; ++i )
        for ( unsigned int j = 0; j < y; ++j )
            locations( i, j ) = randomBool() ? LocationState::CLEAN : LocationState::DIRTY;
}

std::unique_ptr<Environment> BasicVacuumEnvironment::clone() {
    TRACE;

    return std::unique_ptr<core::Environment>( new BasicVacuumEnvironment( *this ));
}

std::vector<Location> BasicVacuumEnvironment::getAgentLocationsList() const {
    const auto& agentLocationsMap = getAgentLocations();
    std::vector<Location> locations;
    locations.reserve( agentLocationsMap.size());
    std::transform( agentLocationsMap.begin(), agentLocationsMap.end(), std::back_inserter( locations ),
                    []( auto pair ) { return pair.second; } );
    std::sort( locations.begin(), locations.end(), std::less<Location>{} );
    return locations;
}

std::unique_ptr<Percept> BasicVacuumEnvironment::getPerceptSeenBy( const Agent& agent ) {
    TRACE;

    auto& location = getAgentLocation( agent );
    auto state = getLocationState( location );
    return std::make_unique<LocalVacuumEnvironmentPercept>( location, state );
}

void BasicVacuumEnvironment::removeAgent( const Agent& agent ) {
    TRACE;

    agentLocations.erase( agent );
    Environment::removeAgent( agent );
}

bool BasicVacuumEnvironment::isDone() const {
    return agentStopped() || getStepCount() >= maxSteps() || Environment::isDone();
}

LocationState BasicVacuumEnvironment::getLocationState( const Location& location ) const {
    auto[x, y] = location;
    return locations( x, y );
}

void BasicVacuumEnvironment::setLocationState( const Location& location, LocationState state ) {
    auto[x, y] = location;
    locations( x, y ) = state;
}

bool BasicVacuumEnvironment::addAgent( Agent& agent ) {
    TRACE;
    LOG4CPLUS_INFO( GetLogger(),
                    "Agent " << agent.id() << " added to "
                             << util::parseTitle<BasicVacuumEnvironment>() << ' ' << id());

    auto isNew = Environment::addAgent( agent );
    if ( isNew ) setAgentLocation( agent, randomLocation());
    return isNew;
}

void BasicVacuumEnvironment::executeAction( const Agent& agent, const Action& action ) {
    TRACE;
    LOG4CPLUS_TRACE( GetLogger(), "Agent " << agent.id() << " executing action: " << action
                                           << " in \"" << util::parseTitle<BasicVacuumEnvironment>() << '"' );

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
    else if ( action.isNoOp()) { agentStopped( true ); }
    else {
        using namespace aima::core::exception;
        BOOST_THROW_EXCEPTION( Exception{} << EnvironmentType( util::parseTitle<BasicVacuumEnvironment>())
                                           << Because( "Unrecognized action received" ));
    }
}

void BasicVacuumEnvironment::createExogenousChange() {
    TRACE;

    int amount = isDone() ? maxSteps() - stepCount : 1;
    ++stepCount;
    for ( unsigned i = 0; i < getX(); ++i )
        for ( unsigned j = 0; j < getY(); ++j )
            if ( locations( i, j ) == LocationState::CLEAN )
                for ( Agent& agent: getAgents())
                    updatePerformanceMeasure( agent, amount );
}

Location BasicVacuumEnvironment::randomLocation() const {
    TRACE;

    return Location{ std::uniform_int_distribution<unsigned long>( 0, getX() - 1 )( randomnessEngine()),
                     std::uniform_int_distribution<unsigned long>( 0, getY() - 1 )( randomnessEngine()) };
}

void BasicVacuumEnvironment::setAgentLocation( const Agent& agent, const Location& location ) {
    TRACE;

    agentLocations[ agent ] = location;
}
