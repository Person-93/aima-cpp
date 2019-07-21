#include "AStarAgent.hpp"
#include <numeric>
#include <sstream>
#include "SearchNode.hpp"
#include "geometry.hpp"
#include "core/registerAgent.hpp"
#include "util/define_logger.hpp"

using namespace aima::path_finder;

AIMA_REGISTER_AGENT( PathFinderAgent, AStarAgent ); // NOLINT(cert-err58-cpp)

DEFINE_LOGGER( AStarAgent )

std::unique_ptr<aima::core::Agent> AStarAgent::clone() const {
    return std::unique_ptr<aima::core::Agent>{ new AStarAgent{ *this }};
}

namespace {
    float estimateCost( const std::shared_ptr<SearchNode>& node, const Point& goal ) {
        return node->pathCost + distance( node->location, goal );
    }
}

AStarAgent::Generator AStarAgent::search( Point currentLocation,
                                          Point goal,
                                          const PathFinderEnvironment::Obstacles& obstacles ) {
    auto rootNode = makeNode<SearchNode>( {}, currentLocation, 0 );
    plan = rootNode;

    while ( plan->location != goal ) {
        co_yield SearchResults::BUSY;

        {
            const auto& points = triviallyReachablePoints( plan->location, obstacles, goal );
            std::ostringstream ss;
            ss << points.size() << " trivially reachable points: ";

            for ( const auto& point: points ) {
                if ( auto parent = plan->parent.lock(); parent && point == parent->location ) continue;
                ss << point << ", ";
                plan->addChild( makeNode<SearchNode>( plan, point, plan->pathCost + distance( plan->location, point )));
            }
            LOG4CPLUS_DEBUG( GetLogger(), ss.str());
        }

        plan = std::accumulate( plan->children.begin(), plan->children.end(),
                                std::shared_ptr<SearchNode>{},
                                [ &goal ]( std::shared_ptr<SearchNode> a,
                                           std::shared_ptr<SearchNode> b ) {
                                    if ( !a ) return b;
                                    return estimateCost( a, goal ) < estimateCost( b, goal ) ? a : b;
                                } );

        LOG4CPLUS_DEBUG( GetLogger(),
                         "Selected " << plan->location << " distance to goal: " << distance( plan->location, goal ));
    }

    co_yield SearchResults::SUCCESS;
}
