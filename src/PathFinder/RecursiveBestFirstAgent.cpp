#include "RecursiveBestFirstAgent.hpp"
#include <sstream>
#include "geometry.hpp"
#include "core/registerAgent.hpp"
#include "util/define_logger.hpp"

using namespace aima::path_finder;

AIMA_REGISTER_AGENT( PathFinderAgent, RecursiveBestFirstAgent ); // NOLINT(cert-err58-cpp)

DEFINE_LOGGER( RecursiveBestFirstAgent )

std::unique_ptr<aima::core::Agent> RecursiveBestFirstAgent::clone() const {
    return std::unique_ptr<aima::core::Agent>{ new RecursiveBestFirstAgent{ *this }};
}

PathFinderAgent::Generator RecursiveBestFirstAgent::search( Point currentLocation,
                                                            Point goal,
                                                            const PathFinderEnvironment::Obstacles& obstacles ) {
    float    dummyVar{};
    co_yield recursiveBestFirstSearch( makeNode<CollapsibleSearchNode>( {}, currentLocation, 0 ),
                                       goal,
                                       obstacles,
                                       std::numeric_limits<float>::infinity(),
                                       dummyVar );
}

namespace {
    float estimateCost( const std::shared_ptr<SearchNode>& node, const Point& goal ) {
        return node->pathCost + distance( node->location, goal );
    }
}

PathFinderAgent::Generator
RecursiveBestFirstAgent::recursiveBestFirstSearch( const std::shared_ptr<CollapsibleSearchNode>& node,
                                                   const Point& goal,
                                                   const PathFinderEnvironment::Obstacles& obstacles,
                                                   float maxEstimateAllowed,
                                                   float& newMaxAllowed ) {
    LOG4CPLUS_DEBUG( GetLogger(),
                     "Expanding node: " << node->location << " MaxAllowedEstimate: " << maxEstimateAllowed );
    if ( node->location == goal ) {
        co_yield SearchResults::SUCCESS;
        co_return;
    }

    std::vector<std::shared_ptr<CollapsibleSearchNode>> successors;
    for ( const auto& point: triviallyReachablePoints( node->location, obstacles, goal )) {
        if ( auto parent = node->parent.lock(); parent && parent->location == point ) continue;
        successors.push_back(
                makeNode<CollapsibleSearchNode>( node, point, node->pathCost + distance( node->location, point )));
    }

    if ( successors.empty()) {
        co_yield SearchResults::FAIL;
        co_return;
    }

    for ( auto& successor: successors ) {
        successor->estimate = std::max( estimateCost( successor, goal ), node->estimate );
    }

    std::sort( successors.begin(), successors.end(),
               []( const std::shared_ptr<CollapsibleSearchNode>& a,
                   const std::shared_ptr<CollapsibleSearchNode>& b ) {
                   return a->estimate < b->estimate;
               } );

    {
        std::ostringstream ss;
        ss << "Successors: ";
        for ( const auto& successor: successors ) {
            ss << successor->location << ": " << successor->estimate << ", ";
        }
        LOG4CPLUS_DEBUG( GetLogger(), ss.str());
    }

    co_yield SearchResults::BUSY;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma ide diagnostic ignored "InfiniteRecursion"
    while ( true ) {
        auto& best = successors[ 0 ];
        plan = best;
        if ( best->estimate > maxEstimateAllowed ) {
            newMaxAllowed = best->estimate;
            co_yield SearchResults::FAIL;
            co_return;
        }

        float newMaxEstimateAllowed = successors.size() == 1 ? best->estimate : std::min( maxEstimateAllowed,
                                                                                          successors[ 1 ]->estimate );

        auto search = recursiveBestFirstSearch( best, goal, obstacles, newMaxEstimateAllowed, best->estimate );

        for ( auto result: search ) {
            switch ( result ) {
                case SearchResults::SUCCESS: co_yield SearchResults::SUCCESS;
                    co_return;
                case SearchResults::BUSY: co_yield SearchResults::BUSY;
                    break;
                case SearchResults::FAIL: break;
            }
        }

        std::sort( successors.begin(), successors.end(),
                   []( const std::shared_ptr<CollapsibleSearchNode>& a,
                       const std::shared_ptr<CollapsibleSearchNode>& b ) {
                       return a->estimate < b->estimate;
                   } );
    }
#pragma clang diagnostic pop
}
