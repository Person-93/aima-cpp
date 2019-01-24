#include <core/Action.hpp>

#include "BasicVacuumAgent.hpp"
#include "LocalVacuumEnvironmentPercept.hpp"
#include "BasicVacuumEnvironment.hpp"

using namespace aima::core;

namespace aima::vacuum {
    const Action& BasicVacuumAgent::execute( const Percept& percept ) {
        auto p = dynamic_cast<const LocalVacuumEnvironmentPercept*>(&percept);
        if ( !p ) throw std::out_of_range( "Agent received percept of unrecognized type" );
        if ( p->agentLocationState == LocationState::DIRTY ) return BasicVacuumEnvironmentBase::ACTION_SUCK;
        if ( p->agentLocation == Location{ 0, 0 } ) return BasicVacuumEnvironmentBase::ACTION_MOVE_RIGHT;
        if ( p->agentLocation == Location{ 1, 0 } ) return BasicVacuumEnvironmentBase::ACTION_MOVE_LEFT;
        return Action::noOp();
    }
}
