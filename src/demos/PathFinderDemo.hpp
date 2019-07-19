#pragma once

#include "core/Demo.hpp"
#include <iosfwd>
#include "PathFinder/IterativeDeepeningAgent.hpp"
#include "PathFinder/PathFinderEnvironment.hpp"

namespace aima::core {
    class Environment;
}

namespace aima::demos {
    class PathFinderDemo : public core::Demo {
    public:
        explicit PathFinderDemo( std::ostream& out );

    protected:
        void init() override;

        void runAtEnd() override;

        core::Environment& environment() noexcept override { return pathFinderEnvironment; }

    private:
        path_finder::PathFinderEnvironment   pathFinderEnvironment;
        path_finder::IterativeDeepeningAgent pathFinderAgent;
    };
}
