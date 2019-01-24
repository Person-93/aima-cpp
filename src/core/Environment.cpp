#include <functional>

#include "Environment.hpp"
#include "Percept.hpp"
#include "Action.hpp"

using std::ref;

namespace aima::core {
    const Environment::Agents& Environment::getAgents() const { return agents; }

    bool Environment::addAgent( Agent& agent ) {
        auto[_, isNew] = agents.insert( agent );
        if ( isNew ) notifyEnvironmentViews( agent );
        return isNew;
    }

    void Environment::removeAgent( const Agent& agent ) {
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
        std::for_each( agents.begin(), agents.end(),
                       [ this ]( Agent& agent ) {
                           if ( agent.isAlive()) {
                               auto percept = getPerceptSeenBy( agent );
                               auto action  = agent.execute( *percept );
                               executeAction( agent, action );
                               notifyEnvironmentViews( agent, *percept, action );
                           }
                       } );
        createExogenousChange();
    }

    void Environment::step( unsigned int n ) {
        for ( int i = 0; i < n && !isDone(); ++i ) step();
    }

    void Environment::stepUntilDone() {
        while ( !isDone()) step();
    }

    bool Environment::isDone() const {
        // TODO implement cooperative cancellation of Environment
        return !std::any_of( agents.cbegin(), agents.cend(), std::mem_fn( &Agent::isAlive ));
    }

    double Environment::getPerformanceMeasure( const Agent& agent ) {
        return performanceMeasures[ ref( agent ) ];
    }

    void Environment::addEnvironmentView( EnvironmentView& view ) {
        views.insert( view );
    }

    void Environment::removeEnvironmentView( const EnvironmentView& view ) {
        views.erase( *const_cast<EnvironmentView*>(&view));
    }

    void Environment::notifyViews( std::string_view message ) {
        std::for_each( views.begin(), views.end(), [ & ]( EnvironmentView& view ) {
            view.notify( message );
        } );
    }

    void Environment::updatePerformanceMeasure( const Agent& agent, double change ) {
        performanceMeasures[ agent ] += change;
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

    const Environment::EnvironmentObjects& Environment::getEnvironmentObjects() const {
        return objects;
    }

    std::unique_ptr<Percept> Environment::getPerceptSeenBy( const Agent& agent ) {
        return std::make_unique<Percept>();
    }

    unsigned Environment::getStepCount() const noexcept { return stepCount; }
}
