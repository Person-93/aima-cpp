#include "Environment.hpp"
#include <functional>
#include <chrono>
#include <thread>
#include <algorithm>
#include "EnvironmentDestroyedView.hpp"
#include "util/define_logger.hpp"
#include "core/Percept.hpp" // IWYU pragma: keep


using namespace aima::core;
using namespace aima::util;
using std::ref;
using std::lock_guard;

DEFINE_LOGGER( Environment );

bool Environment::addAgent( Agent& agent ) {
    TRACE;

    auto[_, isNew] = agents.insert( agent );
    if ( isNew ) {
        performanceMeasures.emplace( agent, 0. );
        notifyEnvironmentViews( agent );
    }
    return isNew;
}

void Environment::removeAgent( const Agent& agent ) {
    TRACE;

    performanceMeasures.erase( agent );
    agents.erase( *const_cast<Agent*>(&agent));
}

void Environment::addEnvironmentObject( EnvironmentObject& object ) {
    TRACE;

    objects.insert( object );
}

void Environment::removeEnvironmentObject( const EnvironmentObject& object ) {
    TRACE;

    objects.erase( *const_cast<EnvironmentObject*> (&object));
}

void Environment::step() {
    TRACE;

    lock_guard lock( mutex );
    locklessStep();
}

namespace aima::core {
    class StepGuard {
    public:
        explicit StepGuard( Environment& env ) noexcept : env( env ), active( !env.stepping ) {
            env.stepping = true;
        }

        ~StepGuard() {
            env.stepping   = false;
            env.shouldStop = false;
        }

    private:
        Environment& env;
        bool active;
    };
}

void Environment::step( unsigned int n, unsigned int delay ) {
    TRACE;

    StepGuard g( *this );

    if ( delay )
        for ( int i = 0; i < n && !isDone() && !shouldStop; ++i ) {
            step();
            std::this_thread::sleep_for( std::chrono::milliseconds( delay ));
        }
    else {
        lock_guard lock( mutex );
        for ( int  i = 0; i < n && !isDone() && !shouldStop; ++i ) locklessStep();
    }
}

void Environment::stepUntilDone( unsigned int delay ) {
    TRACE;

    StepGuard g( *this );

    if ( delay ) {
        while ( !isDone() && !shouldStop ) {
            step();
            std::this_thread::sleep_for( std::chrono::milliseconds( delay ));
        }
    }
    else {
        lock_guard lock( mutex );
        while ( !isDone() && !shouldStop ) locklessStep();
    }
}

bool Environment::isDone() const {
    return !std::any_of( agents.cbegin(), agents.cend(), []( Agent& agent ) { return agent.isAlive(); } );
}

void Environment::notifyViews( std::string_view message ) {
    TRACE;
    for ( EnvironmentView& view: views ) view.notify( message );
}

Environment::~Environment() {
    TRACE;
    for ( EnvironmentDestroyedView& view: destroyedViews ) view.notify();
}

void Environment::notifyEnvironmentViews( const Agent& agent ) {
    TRACE;
    for ( EnvironmentView& view: views ) view.agentAdded( agent, *this );
}

void Environment::notifyEnvironmentViews( const Agent& agent, const Percept& percept, const Action& action ) {
    TRACE;
    for ( EnvironmentView& view: views ) view.agentActed( agent, percept, action, *this );
}

void Environment::locklessStep() {
    TRACE;

    if ( isDone()) return;

    for ( Agent& agent: agents ) {
        if ( agent.isAlive()) {
            const auto percept = getPerceptSeenBy( agent );
            const auto& action = agent.execute( *percept );
            executeAction( agent, action );
            notifyEnvironmentViews( agent, *percept, action );
        }
    }
    createExogenousChange();
}

Environment::Environment( const Environment& other ) : objects( other.objects ), agents( other.agents ) { TRACE; }

Environment::Environment() { TRACE; }
