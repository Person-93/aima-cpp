#include "SearchBasedAgent.hpp"
#include "util/define_logger.hpp"
#include "core/Exception.hpp"
#include "core/Action.hpp"
#include "VisibleVacuumEnvironment.hpp"

using namespace aima::core;
using namespace aima::vacuum;

DEFINE_LOGGER( SearchBasedAgent )

std::unique_ptr<Action> SearchBasedAgent::execute( const Percept& percept ) {
    auto p = dynamic_cast<const GlobalVacuumEnvironmentPercept*>(&percept);
    if ( !p ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << Because{ "Received percept of unrecognized type" } );
    }

    if ( madePlan )
        setDestination( p->agentLocation );

    if ( p->agentLocationState == LocationState::DIRTY ) {
        return VisibleVacuumEnvironment::ACTION_SUCK.clone();
    }

    if ( madePlan )
        return executePlan( p->agentLocation );

    return continuePlanning( *p );
}

std::unique_ptr<Action> SearchBasedAgent::continuePlanning( const GlobalVacuumEnvironmentPercept& p ) {
    if ( firstCall ) {
        firstCall = false;
        generator = search( p );
        iterator  = generator.begin();
    }
    else {
        ++iterator;
        if ( iterator == generator.end()) {
            madePlan = true;
            setDestination( p.agentLocation );
            return executePlan( p.agentLocation );
        }
    }

    return VisibleVacuumEnvironment::ACTION_PLANNING.clone();
}

void SearchBasedAgent::setDestination( const Location& agentLocation ) {
    if ( !startedExecuting ) {
        startedExecuting = true;

        for ( auto plan = getPlan(); plan; plan = plan->parent ) {
            itinerary.push_back( plan->location );
        }

        itineraryIter = itinerary.crbegin();
    }

    if ( auto dest = nextStop(); dest && agentLocation == *dest ) {
        ++itineraryIter;
    }
}

std::unique_ptr<Action> SearchBasedAgent::executePlan( const Location& agentLocation ) {
    if ( itineraryIter == itinerary.crend()) {
        isAlive( false );
        return Action::noOp();
    }

    const auto& destination = *itineraryIter;

    if ( agentLocation.x > destination.x )
        return VisibleVacuumEnvironment::ACTION_MOVE_LEFT.clone();
    if ( agentLocation.x < destination.x )
        return VisibleVacuumEnvironment::ACTION_MOVE_RIGHT.clone();
    if ( agentLocation.y > destination.y )
        return VisibleVacuumEnvironment::ACTION_MOVE_UP.clone();
    if ( agentLocation.y < destination.y )
        return VisibleVacuumEnvironment::ACTION_MOVE_DOWN.clone();

    AIMA_THROW_EXCEPTION( std::logic_error{ "Agent at destination" } );
}
