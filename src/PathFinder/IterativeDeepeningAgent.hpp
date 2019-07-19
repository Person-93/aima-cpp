#pragma once

#include <functional>
#include <atomic>
#include <optional>
#include <memory>
#include <cppcoro/generator.hpp>
#include "PathFinderAgent.hpp"
#include "SearchNode.hpp"

namespace aima::path_finder {
    class IterativeDeepeningAgent : public PathFinderAgent {
    public:
        IterativeDeepeningAgent() = default;

        IterativeDeepeningAgent( IterativeDeepeningAgent&& other ) = delete;

        std::unique_ptr<Agent> clone() const override;

        std::shared_ptr<const SearchNode> getPlan() const override { return plan.lock(); }

    protected:
        /**
         * Protected to prevent slicing
         * @param other
         */
        IterativeDeepeningAgent( const IterativeDeepeningAgent& other ) = default;

    private:
        Generator search( Point currentLocation,
                          Point goal,
                          const PathFinderEnvironment::Obstacles& obstacles ) override;


        Generator iterativeDeepeningSearch( std::shared_ptr<SearchNode> node,
                                            Point goal,
                                            const PathFinderEnvironment::Obstacles& obstacles );

        Generator depthLimitedSearch( const std::shared_ptr<SearchNode>& node,
                                      Point goal,
                                      const PathFinderEnvironment::Obstacles& obstacles,
                                      size_t limit );

        std::weak_ptr<SearchNode> plan{};
    };
}
