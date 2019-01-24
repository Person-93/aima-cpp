#pragma once

#include <functional>
#include <sstream>

#include <core/Agent.hpp>
#include <core/Action.hpp>
#include <core/Percept.hpp>

#include "LocalVacuumEnvironmentPercept.hpp"
#include "BasicVacuumEnvironment.hpp"

namespace aima::vacuum {
    namespace {
        using namespace core;
    }

    template< unsigned X, unsigned Y >
    class StatefulVacuumAgent : public core::Agent {
    public:
        const Action& execute( const Percept& percept ) override {
            auto p = dynamic_cast<const LocalVacuumEnvironmentPercept*>(&percept);
            if ( !p ) throw std::out_of_range( "Agent received percept of unrecognized type" );
            auto location = p->agentLocation;
            state.at( location.x ).at( location.y ) = true;
            if ( p->agentLocationState == LocationState::DIRTY ) return BasicVacuumEnvironmentBase::ACTION_SUCK;
            if ( allClean()) return Action::noOp();
            if ( location == Location{ 0, 0 } ) return BasicVacuumEnvironmentBase::ACTION_MOVE_RIGHT;
            if ( location == Location{ 1, 0 } ) return BasicVacuumEnvironmentBase::ACTION_MOVE_LEFT;
            else {
                std::ostringstream s;
                s << percept;
                throw std::out_of_range( "Received unrecognized percept: " + s.str());
            }
        }

    private:
        bool allClean() {
            return std::all_of( state.begin(), state.end(), []( const std::array<bool, Y>& array ) {
                return std::all_of( array.begin(), array.end(), []( bool b ) { return b; } );
            } );
        }

        std::array<std::array<bool, Y>, X> state{ false };
    };
}
