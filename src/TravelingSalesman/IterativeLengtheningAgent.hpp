#pragma once

#include "core/Agent.hpp"
#include <cppcoro/generator.hpp>
#include "util/geometry/Point.hpp"
#include "SearchNode.hpp"
#include "TravelingSalesmanEnvironment.hpp"
#include "util/ThreadSafeWrapper.hpp"

namespace aima::traveling_salesman {
    struct AgentStatus {
        size_t nodesInMemory{};
        size_t maxNodesInMemory{};
        size_t nodesGenerated{};
        size_t timeSpent{};
        float  pathLength{};
    };

    class IterativeLengtheningAgent final : public core::Agent {
    public:
        IterativeLengtheningAgent() = default;

        IterativeLengtheningAgent( const IterativeLengtheningAgent& other ) : Agent{ *this } {}

        std::unique_ptr<Agent> clone() const override {
            return std::unique_ptr<Agent>{ new IterativeLengtheningAgent{ *this }};
        }

        std::unique_ptr<core::Action> execute( const core::Percept& percept ) override;

        std::shared_ptr<SearchNode> getPlan() const { return plan; }

        AgentStatus getStatus() const { return status.dirtyRead(); }

    private:
        enum class SearchStatus { BUSY, SUCCESS, FAIL };

        using Generator = cppcoro::generator<SearchStatus>;

        Generator iterativeLengthLimitedSearch( const TravelingSalesmanEnvironment::Locations& locations );

        IterativeLengtheningAgent::Generator
        lengthLimitedSearch( const TravelingSalesmanEnvironment::Locations& locations,
                             float limit,
                             float& returnNewLimit );

        /**
         * Creates a search node and updates the status to reflect the new search node count
         * @param parent
         * @param location
         * @param cost
         * @return
         */
        std::shared_ptr<SearchNode>
        makeNode( std::shared_ptr<SearchNode> parent, std::vector<size_t> visited, float cost );

        util::ThreadSafeWrapper<AgentStatus> status;
        Generator                            generator;
        Generator::iterator                  iterator;
        std::shared_ptr<SearchNode>          plan;
        bool                                 firstCall{ true };
    };
}
