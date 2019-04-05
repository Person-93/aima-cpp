#pragma once

#include <memory>
#include <string_view>
#include "util/UniqueIdMixin.hpp"

namespace aima::core {
    class Environment;

    class Agent;

    class Percept;

    class Action;

    /**
     * Represents a view on an environment. Objects can receive notifications when events take place in the environment
     */
    class EnvironmentView : public util::UniqueIdMixin<EnvironmentView> {
    public:
        EnvironmentView() = default;

        EnvironmentView( const EnvironmentView& ) = delete;

        EnvironmentView( EnvironmentView&& ) = delete;

        EnvironmentView& operator=( const EnvironmentView& ) = delete;

        EnvironmentView& operator=( EnvironmentView&& ) = delete;

        /**
         * This method will be called when the environment has an arbitrary message for its viewers
         * @param message The message from the environment
         */
        virtual void notify( std::string_view message ) = 0;

        /**
         * This method will be called from the environment's destructor
         */
        virtual void notify( const Environment& source ) noexcept {}

        /**
         * This method will be called when an agent is added to the environment
         * @param agent The agent that was added
         * @param source The environment that the agent was added to
         */
        virtual void agentAdded( const Agent& agent, const Environment& source ) = 0;

        /**
         * This method will be called when an agent acts
         * @param agent The agent that acted
         * @param percept What the agent perceived
         * @param action The action that the agent performed
         * @param environment The environment that the agent acted in
         */
        virtual void agentActed( const Agent& agent,
                                 const Percept& percept,
                                 const Action& action,
                                 const Environment& environment ) = 0;

    protected:
        /**
         * This method is protected to prevent slicing of derived classes
         */
        ~EnvironmentView() = default;
    };
}
