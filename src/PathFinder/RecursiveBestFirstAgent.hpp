#pragma once

#include "PathFinderAgent.hpp"
#include "CollapsibleSearchNode.hpp"

namespace aima::path_finder {
    class RecursiveBestFirstAgent final : public PathFinderAgent {
    public:
        std::unique_ptr<core::Agent> clone() const override;

        std::shared_ptr<const SearchNode> getPlan() const override { return plan; }

    private:
        Generator search( Point currentLocation,
                          Point goal,
                          const PathFinderEnvironment::Obstacles& obstacles ) override;

        PathFinderAgent::Generator recursiveBestFirstSearch( const std::shared_ptr<CollapsibleSearchNode>& node,
                                                             const Point& goal,
                                                             const PathFinderEnvironment::Obstacles& obstacles,
                                                             float maxEstimateAllowed,
                                                             float& newMaxAllowed );

        std::shared_ptr<CollapsibleSearchNode> plan;
    };
}
