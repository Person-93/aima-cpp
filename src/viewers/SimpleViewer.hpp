#pragma once

#include <iosfwd>
#include <string_view>

#include "core/EnvironmentView.hpp"

namespace aima::core {
    class Agent;

    class Percept;

    class Action;
}

namespace aima::viewer {
    namespace {
        using namespace aima::core;
    }
    class SimpleViewer : public EnvironmentView {
    public:
        explicit SimpleViewer( std::ostream& out );

        void notify( std::string_view message ) override;

        void agentAdded( const Agent& agent, const Environment& source ) override;

        void agentActed( const Agent& agent,
                         const Percept& percept,
                         const Action& action,
                         const Environment& environment ) override;

    private:
        std::ostream& out;
    };
}
