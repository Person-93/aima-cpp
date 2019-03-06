#pragma once

#include <iosfwd>
#include <string_view>

#include "core/EnvironmentView.hpp"

namespace aima::core {
    class Agent;

    class Percept;

    class Action;

    class Environment;
}

namespace aima::viewer {
    class SimpleViewer : public core::EnvironmentView {
    public:
        explicit SimpleViewer( std::ostream& out );

        void notify( std::string_view message ) override;

        void agentAdded( const core::Agent& agent, const core::Environment& source ) override;

        void agentActed( const core::Agent& agent,
                         const core::Percept& percept,
                         const core::Action& action,
                         const core::Environment& environment ) override;

    private:
        std::ostream& out;
    };
}
