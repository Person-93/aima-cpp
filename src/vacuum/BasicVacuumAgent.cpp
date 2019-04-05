#include "BasicVacuumAgent.hpp"
#include "LocalVacuumEnvironmentPercept.hpp"
#include "BasicVacuumEnvironment.hpp"
#include "util/parseTitle.hpp"
#include "core/Action.hpp"
#include "core/Exception.hpp"
#include "misc.hpp"
#include "util/define_logger.hpp"

using namespace aima::core;
using namespace aima::vacuum;
using aima::vacuum::BasicVacuumAgent;

DEFINE_LOGGER( BasicVacuumAgent );

BasicVacuumAgent::BasicVacuumAgent() {
    TRACE;
}

std::unique_ptr<Agent> BasicVacuumAgent::clone() const {
    TRACE;
    return std::unique_ptr<Agent>( new BasicVacuumAgent( *this ));
}

const Action& BasicVacuumAgent::execute( const Percept& percept ) {
    TRACE;

    auto p = dynamic_cast<const LocalVacuumEnvironmentPercept*>(&percept);
    if ( !p ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << AgentType( util::parseTitle<BasicVacuumAgent>())
                                          << Because( "Agent received percept of unrecognized type" ));
    }
    if ( p->agentLocationState == LocationState::DIRTY ) return BasicVacuumEnvironment::ACTION_SUCK;
    if ( p->agentLocation == Location{ 0, 0 } ) return BasicVacuumEnvironment::ACTION_MOVE_RIGHT;
    if ( p->agentLocation == Location{ 1, 0 } ) return BasicVacuumEnvironment::ACTION_MOVE_LEFT;
    return Action::noOp();
}

