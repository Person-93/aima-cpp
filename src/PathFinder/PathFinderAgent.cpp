#include "PathFinderAgent.hpp"
#include <utility>
#include "util/geometry/Point.hpp"
#include "util/define_logger.hpp"
#include "core/Exception.hpp"
#include "core/Action.hpp"
#include "PathFinderPercept.hpp"
#include "SearchNode.hpp"
#include "util/geometry/geometry.hpp"

using namespace aima::core;
using namespace aima::path_finder;
using aima::util::geometry::Point;

DEFINE_LOGGER( PathFinderAgent )

std::unique_ptr<Action> PathFinderAgent::execute( const Percept& percept ) {
    auto p = dynamic_cast<const PathFinderPercept*>(&percept);
    if ( !p ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << Because( "Agent received unrecognized percept" ));
    }

    LOG4CPLUS_DEBUG( GetLogger(), "Agent " << id() << " searching..." );
    if ( firstCall ) {
        firstCall = false;
        generator = search( p->location, p->goal, p->obstacles );
        iterator  = generator.begin();
    }
    else
        ++iterator;

    if ( iterator == generator.end()) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << Because{ "Search already completed" } );
    }

    status.access( [ this ]( AgentStatus& status ) {
        ++status.timeSpent;
        const auto& plan = getPlan();
        status.pathLength = plan ? getPlan()->pathCost : 0;
    } );

    switch ( *iterator ) {
        case SearchResults::BUSY: return PathFinderEnvironment::PLANNING.clone();
        case SearchResults::FAIL: return PathFinderEnvironment::FAILED.clone();
        case SearchResults::SUCCESS: {
            isAlive( false );
            return PathFinderEnvironment::SUCCEEDED.clone();
        }
    }
}

static bool isTriviallyReachable( const Point& start,
                                  const Point& endPoint,
                                  const PathFinderEnvironment::Obstacles& obstacles ) {
    using namespace aima::util::geometry;
    LineSegment straightToFinish{ start, endPoint };
    return std::none_of( obstacles.begin(), obstacles.end(), [ &straightToFinish ]( const Polygon& obstacle ) {
        return isCutBy( obstacle, straightToFinish );
    } );
}

std::vector<Point> PathFinderAgent::triviallyReachablePoints( const Point& currentLocation,
                                                              const PathFinderEnvironment::Obstacles& obstacles,
                                                              const Point& goal ) {
    std::vector<Point> points{};
    for ( const auto& obstacle: obstacles ) {
        std::copy_if( obstacle.getPoints().begin(), obstacle.getPoints().end(), std::back_inserter( points ),
                      [ & ]( const Point& point ) {
                          return point != currentLocation && isTriviallyReachable( currentLocation, point, obstacles );
                      } );
    }
    if ( isTriviallyReachable( currentLocation, goal, obstacles ))
        points.push_back( goal );
    return points;
}
