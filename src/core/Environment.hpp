#pragma once

#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <mutex>
#include <functional>
#include <string_view>

#include "util/ThreadSafeWrapper.hpp" // IWYU pragma: keep
#include "EnvironmentObject.hpp"
#include "Agent.hpp"
#include "EnvironmentView.hpp"

// IWYU pragma: no_include "util/UniqueIdMixin.hpp"

namespace aima::core {
    class Action;

    class Percept;

    /** An environment in which agents can perceive and act */
    class Environment {
    public:
        using Agents              = std::unordered_set<std::reference_wrapper<Agent>, typename Agent::hash>;
        using EnvironmentObjects  = std::unordered_set<std::reference_wrapper<EnvironmentObject>,
                                                       EnvironmentObject::hash>;
        using PerformanceMeasures = std::unordered_map<std::reference_wrapper<const Agent>,
                                                       util::ThreadSafeWrapper<double>,
                                                       typename Agent::hash>;
        using EnvironmentViews    = std::unordered_set<std::reference_wrapper<EnvironmentView>,
                                                       typename EnvironmentView::hash>;


        /**
         * @return The agents that exist in the environment
         */
        const Agents& getAgents() const { return agents; }

        /**
         * @brief Add an agent to the environment.
         *
         * The environment does not own the agent.
         * It is the user's responsibility to ensure that the agent doesn't go out of scope before
         * the environment does
         *
         * @param agent The agent to be added to the environment
         * @return <code>true</code> if it's a new agent, <code>false</code> if it was already inserted.
         */
        virtual bool addAgent( Agent& agent );

        virtual void addAgent( Agent&& agent ) = delete;

        /**
         * Remove an agent from the environment
         *
         * @param agent The agent to be removed
         */
        virtual void removeAgent( const Agent& agent );

        virtual void executeAction( const Agent& agent, const Action& action ) = 0;

        virtual std::unique_ptr<Percept> getPerceptSeenBy( const Agent& agent ) = 0;

        virtual void createExogenousChange() {}

        /** @return All of the environment objects that exist in the environment except the agents */
        const EnvironmentObjects& getEnvironmentObjects() const noexcept { return objects; }

        /**
         * Adds an object to the environment
         *
         * The environment does not own the object.
         * It is the user's responsibility to ensure that the object doesn't go out of scope before
         * the environment does
         *
         * @param object The object to be added to the environment
         */
        void addEnvironmentObject( EnvironmentObject& object );

        void addEnvironmentObject( EnvironmentObject&& object ) = delete;

        /**
         * Remove an object from the environment
         *
         * @param object The object to be removed
         */
        void removeEnvironmentObject( const EnvironmentObject& object );

        /** Move the environment one time step forward */
        void step();

        /**
         * Move the environment n time steps forward
         * @param n The number of steps to move forward
         * @param delay Number of milliseconds to wait between steps
         */
        void step( unsigned int n, unsigned int delay = 0 );

        /**
         * Step through time until the environment has no more tasks
         * @param delay Number of milliseconds to wait between steps
         */
        void stepUntilDone( unsigned int delay = 0 );

        /** Stop moving though time steps */
        void stop() noexcept { shouldStop = true; }

        /**
         * @return <code>true</code> if the Environment is finished with its current task(s),
         *         <code>false</code> otherwise.
         */
        virtual bool isDone() const;

        bool isRunning() const { return stepping; }

        /**
         * Retrieve the performance measure associated with an agent
         *
         * @param agent The agent for which a performance measure is to be retrieved.
         *
         * @return The performance measure associated with the agent
         */
        const util::ThreadSafeWrapper<double>& getPerformanceMeasure( const Agent& agent ) const {
            return performanceMeasures.at( agent );
        }

        const PerformanceMeasures& getPerformanceMeasures() const noexcept { return performanceMeasures; }

        /**
         * Add a view on the environment
         *
         * @param view
         */
        virtual void addEnvironmentView( EnvironmentView& view ) { views.insert( view ); }

        virtual void addEnvironmentView( EnvironmentView&& view ) = delete;

        /**
         * Remove a view on the environment
         *
         * @param view
         */
        virtual void removeEnvironmentView( const EnvironmentView& view ) {
            views.erase( *const_cast<EnvironmentView*>(&view));
        }

        /**
         * Notify all registered viewers of a message
         *
         * @param message The message to send the viewers
         */
        void notifyViews( std::string_view message );

        unsigned getStepCount() const noexcept { return stepCount; }

        /** Destroy the Environment */
        virtual ~Environment() = default;

    protected:
        /**
         * Update the performance measure of an agent
         *
         * Should be a positive number to increase its performance measure
         * @param agent The agent to update
         * @param change The amount to change the performance measure
         */
        void updatePerformanceMeasure( const Agent& agent, double change ) { performanceMeasures[ agent ] += change; }

        /**
         * Notify the views that a new agent was added
         *
         * @param agent The agent that was added
         */
        void notifyEnvironmentViews( const Agent& agent );

        /**
         * Notify the views that an agent acted
         * @param agent The agent that acted
         * @param percept The percept that induced the action
         * @param action The action taken
         */
        void notifyEnvironmentViews( const Agent& agent, const Percept& percept, const Action& action );

        unsigned stepCount             = 0;
    private:
        void locklessStep();

        friend class StepGuard;

        EnvironmentObjects  objects;
        EnvironmentViews    views;
        Agents              agents;
        PerformanceMeasures performanceMeasures;
        std::mutex          mutex;
        std::atomic_bool    stepping   = false;
        std::atomic_bool    shouldStop = false;
    };
}
