#include <sstream>
#include <stdexcept>
#include <string_view>

#include "util/parseTitle.hpp"
#include "core/Percept.hpp"
#include "core/Action.hpp"
#include "BasicVacuumAgent.hpp"
#include "LocalVacuumEnvironmentPercept.hpp"
#include "BasicVacuumEnvironment.hpp"
#include "types.hpp"

using namespace aima::core;

namespace aima::vacuum {
    const Action& BasicVacuumAgent::execute( const Percept& percept ) {
        auto p = dynamic_cast<const LocalVacuumEnvironmentPercept*>(&percept);
        if ( !p ) {
            std::ostringstream s;
            s << util::parseTitle<BasicVacuumAgent>() << " received percept of unrecognized type";
            throw std::out_of_range( s.str());
        }
        if ( p->agentLocationState == LocationState::DIRTY ) return BasicVacuumEnvironment::ACTION_SUCK;
        if ( p->agentLocation == Location{ 0, 0 } ) return BasicVacuumEnvironment::ACTION_MOVE_RIGHT;
        if ( p->agentLocation == Location{ 1, 0 } ) return BasicVacuumEnvironment::ACTION_MOVE_LEFT;
        return Action::noOp();
    }
}
