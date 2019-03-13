#include <functional>
#include <chrono>
#include <thread>
#include <algorithm>

#include "Environment.hpp" // IWYU pragma: associated
#include "Percept.hpp" // IWYU pragma: keep


using namespace aima::core;
using namespace aima::util;
using std::ref;
using std::lock_guard;

bool Environment::addAgent( Agent& agent ) {
    auto[_, isNew] = agents.insert( agent );
    if ( isNew ) {
        performanceMeasures.emplace( agent, 0. );
        notifyEnvironmentViews( agent );
    }
    return isNew;
}

void Environment::removeAgent( const Agent& agent ) {
    performanceMeasures.erase( agent );
    agents.erase( *const_cast<Agent*>(&agent));
}

void Environment::addEnvironmentObject( EnvironmentObject& object ) {
    if ( auto p = dynamic_cast<Agent*>(&object)) addAgent( *p );
    else objects.insert( object );
}

void Environment::removeEnvironmentObject( const EnvironmentObject& object ) {
    if ( auto p = dynamic_cast<const Agent*>(&object)) removeAgent( *p );
    else objects.erase( *const_cast<EnvironmentObject*> (&object));
}

void Environment::step() {
    lock_guard lock( mutex );
    locklessStep();
}

namespace aima::core {
    class StepGuard {
    public:
        explicit StepGuard( Environment& env ) noexcept : env( env ) {
            env.stepping = true;
        }

        ~StepGuard() {
            env.stepping   = false;
            env.shouldStop = false;
        }

    private:
        Environment& env;
    };
}

void Environment::step( unsigned int n, unsigned int delay ) {
    StepGuard g( *this );

    if ( delay )
        for ( int i = 0; i < n && !isDone() && !shouldStop; ++i ) {
            step();
            std::this_thread::sleep_for( std::chrono::milliseconds( delay ));
        }
    else {
        lock_guard l( mutex );
        for ( int  i = 0; i < n && !isDone() && !shouldStop; ++i ) locklessStep();
    }
}

void Environment::stepUntilDone( unsigned int delay ) {
    StepGuard g( *this );

    if ( delay ) {
        while ( !isDone() && !shouldStop ) {
            step();
            std::this_thread::sleep_for( std::chrono::milliseconds( delay ));
        }
    }
    else {
        lock_guard l( mutex );
        while ( !isDone() && !shouldStop ) locklessStep();
    }
}

bool Environment::isDone() const {
    // TODO implement cooperative cancellation of Environment
    return !std::any_of( agents.cbegin(), agents.cend(), std::mem_fn( &Agent::isAlive ));
}

void Environment::notifyViews( std::string_view message ) {
    std::for_each( views.begin(), views.end(), [ & ]( EnvironmentView& view ) {
        view.notify( message );
    } );
}

void Environment::notifyEnvironmentViews( const Agent& agent ) {
    std::for_each( views.begin(), views.end(), [ & ]( EnvironmentView& view ) {
        view.agentAdded( agent, *this );
    } );
}

void Environment::notifyEnvironmentViews( const Agent& agent, const Percept& percept, const Action& action ) {
    std::for_each( views.begin(), views.end(), [ & ]( EnvironmentView& view ) {
        view.agentActed( agent, percept, action, *this );
    } );
}

void Environment::locklessStep() {
    if ( isDone()) return;

    std::for_each( agents.begin(), agents.end(),
                   [ this ]( Agent& agent ) {
                       if ( agent.isAlive()) {
                           auto percept = getPerceptSeenBy( agent );
                           auto& action = agent.execute( *percept );
                           executeAction( agent, action );
                           notifyEnvironmentViews( agent, *percept, action );
                       }
                   } );
    createExogenousChange();
}
