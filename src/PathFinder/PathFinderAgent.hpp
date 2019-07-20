#pragma once

#include "core/Agent.hpp"
#include <memory>
#include <cppcoro/recursive_generator.hpp>
#include "PathFinderEnvironment.hpp"
#include "util/ThreadSafeWrapper.hpp"

namespace aima::core {
    class Percept;

    class Agent;
}

namespace aima::path_finder {
    class Point;

    class SearchNode;

    struct AgentStatus {
        size_t nodesInMemory{};
        size_t maxNodesInMemory{};
        size_t nodesGenerated{};
        size_t timeSpent{};
        float  pathLength{};
    };

    class PathFinderAgent : public core::Agent {
    public:
        PathFinderAgent() = default;

        const core::Action& execute( const core::Percept& percept ) override;

        virtual std::shared_ptr<const SearchNode> getPlan() const = 0;

        AgentStatus getStatus() const { return status.dirtyRead(); }

    protected:
        PathFinderAgent( const PathFinderAgent& other ) : Agent{ *this } {}

        static std::vector<Point>
        triviallyReachablePoints( const Point& currentLocation,
                                  const PathFinderEnvironment::Obstacles& obstacles,
                                  const Point& goal );

        std::shared_ptr<SearchNode> makeNode( std::weak_ptr<SearchNode> parent, Point location, float cost );

        enum class SearchResults {
            SUCCESS, BUSY, FAIL
        };

        using Generator = cppcoro::recursive_generator<SearchResults>;

    private:
        virtual Generator search( Point currentLocation,
                                  Point goal,
                                  const PathFinderEnvironment::Obstacles& obstacles ) = 0;

        util::ThreadSafeWrapper<AgentStatus> status{};
        Generator                            generator{};
        Generator::iterator                  iterator;
        bool                                 firstCall{ true };
    };
}
