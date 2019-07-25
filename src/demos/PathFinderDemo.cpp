#include "PathFinderDemo.hpp"
#include "util/define_logger.hpp"
#include "util/StringBuilder.hpp"
#include "util/geometry/Point.hpp"
#include "core/Exception.hpp"

using namespace aima::core;
using namespace aima::path_finder;
using aima::demos::PathFinderDemo;

DEFINE_LOGGER( PathFinderDemo );

PathFinderDemo::PathFinderDemo( std::ostream& out ) : Demo{ out } { TRACE; }

void PathFinderDemo::init() {
    pathFinderEnvironment.addAgent( pathFinderAgent );
    util::StringBuilder stringBuilder;
    for ( auto& obstacle : pathFinderEnvironment.getObstacles()) {
        stringBuilder << "Obstacle: ";
        stringBuilder << obstacle << '\n';
        getViewer().notify( std::move( stringBuilder ).toString());
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"

void aima::demos::PathFinderDemo::runAtEnd() {
    util::StringBuilder stringBuilder;
    for ( const Agent& agent: pathFinderEnvironment.getAgents()) {
        stringBuilder << "Agent " << agent.id() << " plan: ";
        auto& pathFinderAgent = dynamic_cast<const PathFinderAgent&>(agent);
        for ( auto node = pathFinderAgent.getPlan(); node; node = node->parent.lock() ) {
            stringBuilder << node->location << ' ';
        }
        getViewer().notify( std::move( stringBuilder ).toString());
    }
}

#pragma clang diagnostic pop
