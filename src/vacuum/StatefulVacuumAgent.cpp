#include "StatefulVacuumAgent.hpp"
#include <cstddef>
#include <string>
#include "LocalVacuumEnvironmentPercept.hpp"
#include "BasicVacuumEnvironment.hpp"
#include "util/parseTitle.hpp"
#include "util/define_logger.hpp"
#include "misc.hpp"
#include "core/Action.hpp"
#include "core/Exception.hpp"


using namespace aima::core;
using namespace aima::vacuum;
using std::out_of_range;

DEFINE_LOGGER( StatefulVacuumAgent );

std::unique_ptr<Agent> StatefulVacuumAgent::clone() const {
    TRACE;
    return std::unique_ptr<Agent>( new StatefulVacuumAgent( *this ));
}

StatefulVacuumAgent::StatefulVacuumAgent( unsigned long x, unsigned long y ) : state( x, y ) {
    TRACE;
    LOG4CPLUS_DEBUG( GetLogger(), "Constructing \"" << util::parseTitle<StatefulVacuumAgent>()
                                                    << "\" Dimensions: (" << x << ',' << y << ')' );

    for ( auto i = state.begin1(), end1 = state.end1(); i != end1; ++i )
        for ( bool& j : i )
            j = false;
}

StatefulVacuumAgent::StatefulVacuumAgent( const BasicVacuumEnvironment& environment ) :
        StatefulVacuumAgent( environment.getX(), environment.getY()) { TRACE; }

const Action& StatefulVacuumAgent::execute( const Percept& percept ) {
    TRACE;

    auto p = dynamic_cast<const LocalVacuumEnvironmentPercept*>(&percept);
    if ( !p ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << Because( "Agent received percept of unrecognized type" )
                                          << AgentType( util::parseTitle<StatefulVacuumAgent>()));
    }

    auto location = p->agentLocation;
    state( location.x, location.y ) = true;

    if ( p->agentLocationState == LocationState::DIRTY ) return BasicVacuumEnvironment::ACTION_SUCK;
    if ( allClean()) return Action::noOp();
    if ( location == Location{ 0, 0 } ) return BasicVacuumEnvironment::ACTION_MOVE_RIGHT;
    if ( location == Location{ 1, 0 } ) return BasicVacuumEnvironment::ACTION_MOVE_LEFT;

    using namespace aima::core::exception;
    AIMA_THROW_EXCEPTION( Exception{} << Because( "Agent received unrecognized percept" )
                                      << AgentType( util::parseTitle<StatefulVacuumAgent>())
                                      << PerceptInfo( util::StringBuilder( 256 ) << percept ));
}

bool StatefulVacuumAgent::allClean() {
    for ( auto i = state.cbegin1(), end1 = state.cend1(); i != end1; ++i )
        for ( bool j : i )
            if ( !j ) return false;
    return true;
}
