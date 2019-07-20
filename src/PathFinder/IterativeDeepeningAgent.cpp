#include "IterativeDeepeningAgent.hpp"
#include <sstream>
#include <boost/throw_exception.hpp>
#include "core/Action.hpp"
#include "geometry.hpp"
#include "util/define_logger.hpp"
#include "core/Exception.hpp"
#include "core/registerAgent.hpp"

using namespace aima::path_finder;

AIMA_REGISTER_AGENT( PathFinderAgent, IterativeDeepeningAgent );

DEFINE_LOGGER( IterativeDeepeningAgent )

std::unique_ptr<aima::core::Agent> IterativeDeepeningAgent::clone() const {
    return std::unique_ptr<aima::core::Agent>( new IterativeDeepeningAgent{ *this } );
}

PathFinderAgent::Generator IterativeDeepeningAgent::search( Point currentLocation,
                                                            Point goal,
                                                            const PathFinderEnvironment::Obstacles& obstacles ) {
    return iterativeDeepeningSearch(
            makeNode( {}, currentLocation, 0 ),
            goal,
            obstacles );
}

IterativeDeepeningAgent::Generator
IterativeDeepeningAgent::iterativeDeepeningSearch( std::shared_ptr<SearchNode> node,
                                                   Point goal,
                                                   const PathFinderEnvironment::Obstacles& obstacles ) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    for ( size_t depth = 1; true; ++depth ) {
        LOG4CPLUS_DEBUG( GetLogger(), "Iterative deepening search depth " << depth );
        auto       search = depthLimitedSearch( node, goal, obstacles, depth );
        for ( auto response: search ) {
            switch ( response ) {
                case SearchResults::FAIL: co_yield SearchResults::BUSY;
                    break;
                case SearchResults::SUCCESS: isAlive( false );
                    [[fallthrough]];
                case SearchResults::BUSY: co_yield response;
            }
        }
    }
#pragma clang diagnostic pop
}

IterativeDeepeningAgent::Generator
IterativeDeepeningAgent::depthLimitedSearch( const std::shared_ptr<SearchNode>& node,
                                             Point goal,
                                             const PathFinderEnvironment::Obstacles& obstacles,
                                             size_t limit ) {
    if ( !node )
        BOOST_THROW_EXCEPTION( std::logic_error{ "depth-limited search started with a null search node" } );

    LOG4CPLUS_DEBUG( GetLogger(), "Checking node at " << node->location << " with limit " << limit );
    LOG4CPLUS_DEBUG( GetLogger(), "Depth limited search - Checking if reached goal " << goal );
    if ( node->location == goal ) {
        LOG4CPLUS_DEBUG( GetLogger(), "Reached goal " << goal );
        plan = node;
        co_yield SearchResults::SUCCESS;
        co_return;
    }
    LOG4CPLUS_DEBUG( GetLogger(), "Depth limited search - Checking if reached limit" );
    if ( limit == 0 ) {
        co_yield SearchResults::FAIL;
        co_return;
    }

    const auto         reachablePoints = triviallyReachablePoints( node->location, obstacles, goal );
    std::ostringstream ss;
    ss << reachablePoints.size() << " reachable points: ";
    std::copy( reachablePoints.begin(), reachablePoints.end(), std::ostream_iterator<Point>{ ss, "," } );
    LOG4CPLUS_DEBUG( GetLogger(), ss.str());

    co_yield SearchResults::BUSY;

    for ( const auto& point: reachablePoints ) {
        auto& child = node->addChild( makeNode( node, point, node->pathCost + distance( node->location, point )));
        plan = child;

        co_yield depthLimitedSearch( child, goal, obstacles, limit - 1 );
        node->children = {};
    }

    plan = node;
    co_yield SearchResults::FAIL;
}
