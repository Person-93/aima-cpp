#pragma once

#include "PathFinderAgent.hpp"

namespace aima::path_finder {
    class AStarAgent final : public PathFinderAgent {
        std::unique_ptr<core::Agent> clone() const override;

        std::shared_ptr<const SearchNode> getPlan() const override { return plan; }

    private:
        Generator search( util::geometry::Point currentLocation,
                          util::geometry::Point goal,
                          const PathFinderEnvironment::Obstacles& obstacles ) override;

        std::shared_ptr<SearchNode> plan;
    };
}
