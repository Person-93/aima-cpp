#pragma once

#include "util/UniqueIdMixin.hpp"

namespace aima::core {
    class Environment;

    class Agent;

    class Percept;

    class Action;

    class EnvironmentView : public util::UniqueIdMixin<EnvironmentView> {
    public:
        virtual void notify( std::string_view message ) = 0;

        virtual void agentAdded( const Agent& agent, const Environment& source ) = 0;

        virtual void agentActed( const Agent& agent,
                                 const Percept& percept,
                                 const Action& action,
                                 const Environment& environment ) = 0;

        virtual ~EnvironmentView() = default;
    };
}
