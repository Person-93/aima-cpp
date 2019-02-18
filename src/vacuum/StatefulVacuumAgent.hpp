#pragma once

#include <functional>
#include <sstream>

#include "util/parseTitle.hpp"
#include "core/Agent.hpp"
#include "core/Action.hpp"
#include "core/Percept.hpp"
#include "LocalVacuumEnvironmentPercept.hpp"
#include "BasicVacuumEnvironment.hpp"

namespace aima::vacuum {
    namespace {
        using namespace core;
    }

    class StatefulVacuumAgent : public core::Agent {
    public:
        StatefulVacuumAgent( unsigned x, unsigned y ) : x( x ), y( y ), state( x, y ) {}

        const Action& execute( const Percept& percept ) override {
            auto p = dynamic_cast<const LocalVacuumEnvironmentPercept*>(&percept);
            if ( !p ) throw std::out_of_range( "Agent received percept of unrecognized type" );
            auto location = p->agentLocation;
            state( location.x, location.y ) = true;
            if ( p->agentLocationState == LocationState::DIRTY ) return BasicVacuumEnvironment::ACTION_SUCK;
            if ( allClean()) return Action::noOp();
            if ( location == Location{ 0, 0 } ) return BasicVacuumEnvironment::ACTION_MOVE_RIGHT;
            if ( location == Location{ 1, 0 } ) return BasicVacuumEnvironment::ACTION_MOVE_LEFT;
            else {
                std::ostringstream s;
                s << util::parseTitle<StatefulVacuumAgent>() << " received unrecognized percept: " << percept;
                throw std::out_of_range( s.str());
            }
        }

    private:
        bool allClean() {
            for ( unsigned i = 0; i < x; ++i )
                for ( unsigned j = 0; j < y; ++j )
                    if ( !state( i, j )) return false;
            return true;
        }

        boost::numeric::ublas::matrix<bool> state;
        unsigned                            x;
        unsigned                            y;
    };
}
