#include "PathFinderEnvironment.hpp"
#include "PathFinderPercept.hpp"
#include "core/Exception.hpp"
#include "util/define_logger.hpp"
#include "core/Action.hpp"

using namespace aima::core;
using namespace aima::path_finder;
using aima::util::geometry::Polygon;
using aima::util::geometry::Point;

DEFINE_LOGGER( PathFinderEnvironment )

PathFinderEnvironment::PathFinderEnvironment() :
        obstacles{
                Polygon{{{ 225, 10 }, { 285, 95 }, { 240, 205 }, { 145, 180 }, { 130, 95 }}},
                Polygon{{{ 325, 80 }, { 360, 210 }, { 290, 210 }}},
                Polygon{{{ 370, 20 }, { 430, 10 }, { 475, 60 }, { 370, 120 }}},
                Polygon{{{ 490, 20 }, { 600, 20 }, { 600, 190 }, { 490, 190 }}},
                Polygon{{{ 615, 40 }, { 655, 20 }, { 685, 50 }, { 670, 210 }}},
                Polygon{{{ 160, 245 }, { 420, 245 }, { 420, 325 }, { 160, 325 }}},
                Polygon{{{ 460, 285 }, { 515, 235 }, { 430, 165 }}},
                Polygon{{{ 615, 200 }, { 660, 240 }, { 660, 305 }, { 605, 330 }, { 555, 300 }, { 555, 240 }}},
        },
        agentLocations{},
        goal{ 700, 25 } {}

std::unique_ptr<Environment> PathFinderEnvironment::clone() {
    return std::unique_ptr<Environment>( new PathFinderEnvironment( *this ));
}

bool PathFinderEnvironment::addAgent( Agent& agent ) {
    if ( !Environment::addAgent( agent )) return false;
    agentLocations[ agent ] = { 135, 290 }; // TODO generate random or user-specified location for agent start
    return true;
}

void PathFinderEnvironment::removeAgent( const Agent& agent ) {
    agentLocations.erase( agent );
    Environment::removeAgent( agent );
}

std::unique_ptr<Percept> PathFinderEnvironment::getPerceptSeenBy( const Agent& agent ) {
    return std::unique_ptr<Percept>{ new PathFinderPercept{ getObstacles(), getAgentLocation( agent ), getGoal() }};
}

Point PathFinderEnvironment::getAgentLocation( const Agent& agent ) const {
    return getAgentLocations().at( agent );
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

const Action PathFinderEnvironment::PLANNING{ "Planning" };
const Action PathFinderEnvironment::SUCCEEDED{ "Succeeded" };
const Action PathFinderEnvironment::FAILED{ "Failed" };

#pragma clang diagnostic pop