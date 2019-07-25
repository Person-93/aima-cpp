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

namespace aima::util::geometry {
    class Point;
}

namespace aima::path_finder {
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

        std::unique_ptr<core::Action> execute( const core::Percept& percept ) override;

        virtual std::shared_ptr<const SearchNode> getPlan() const = 0;

        AgentStatus getStatus() const { return status.dirtyRead(); }

    protected:
        PathFinderAgent( const PathFinderAgent& other ) : Agent{ *this } {}

        /**
         * Returns all of the points that can be reached without having to navigate around obstacles
         * @param currentLocation
         * @param obstacles
         * @param goal
         * @return
         */
        static std::vector<util::geometry::Point>
        triviallyReachablePoints( const util::geometry::Point& currentLocation,
                                  const PathFinderEnvironment::Obstacles& obstacles,
                                  const util::geometry::Point& goal );

        /**
         * Creates a search node and updates the status to reflect the new search node count
         * @tparam SearchNode
         * @param parent
         * @param location
         * @param cost
         * @return
         */
        template< typename SearchNode >
        std::shared_ptr<SearchNode>
        makeNode( std::weak_ptr<SearchNode> parent, util::geometry::Point location, float cost ) {
            status.access( []( AgentStatus& status ) {
                ++status.nodesInMemory;
                status.maxNodesInMemory = std::max( status.maxNodesInMemory, status.nodesInMemory );
                ++status.nodesGenerated;
            } );
            return std::shared_ptr<SearchNode>{ new SearchNode{ std::move( parent ), location, cost },
                                                [ this ]( SearchNode* p ) {
                                                    status.access( []( AgentStatus& status ) {
                                                        --status.nodesInMemory;
                                                    } );
                                                    delete p;
                                                }};
        }

        enum class SearchResults {
            SUCCESS, BUSY, FAIL
        };

        using Generator = cppcoro::recursive_generator<SearchResults>;

    private:
        virtual Generator search( util::geometry::Point currentLocation,
                                  util::geometry::Point goal,
                                  const PathFinderEnvironment::Obstacles& obstacles ) = 0;

        util::ThreadSafeWrapper<AgentStatus> status{};
        Generator                            generator{};
        Generator::iterator                  iterator;
        bool                                 firstCall{ true };
    };
}
