#pragma once

#include "core/Agent.hpp"
#include <memory>
#include <cppcoro/generator.hpp>
#include "PathFinderEnvironment.hpp"

namespace aima::core {
    class Percept;

    class Agent;
}

namespace aima::path_finder {
    class Point;

    class SearchNode;

    class PathFinderAgent : public core::Agent {
    public:
        PathFinderAgent() = default;

        const core::Action& execute( const core::Percept& percept ) override;

        virtual std::shared_ptr<const SearchNode> getPlan() const = 0;

    protected:
        PathFinderAgent( const PathFinderAgent& other ) : Agent{ *this } {}

        static std::vector<Point>
        triviallyReachablePoints( const Point& currentLocation,
                                  const PathFinderEnvironment::Obstacles& obstacles,
                                  const Point& goal );

        enum class SearchResults {
            SUCCESS, BUSY, FAIL
        };

        using Generator = cppcoro::generator<SearchResults>;

    private:
        virtual Generator search( Point currentLocation,
                                  Point goal,
                                  const PathFinderEnvironment::Obstacles& obstacles ) = 0;

        Generator           generator{};
        Generator::iterator iterator;
        bool                firstCall{ true };
    };
}
