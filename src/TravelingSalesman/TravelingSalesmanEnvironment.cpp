#include "TravelingSalesmanEnvironment.hpp"
#include <random>
#include "util/define_logger.hpp"
#include "core/Exception.hpp"
#include "core/Action.hpp"
#include "TravelingSalesmanPercept.hpp"

using namespace aima::traveling_salesman;
using aima::util::geometry::Point;
using aima::core::Action;

DEFINE_LOGGER( TravelingSalesmanEnvironment )

namespace {
    Point randomPoint() {
        static std::mt19937 engine{ std::random_device{}() };
        return {
                std::uniform_int_distribution<float>{ 0, 700 }( engine ),
                std::uniform_int_distribution<float>{ 0, 500 }( engine ),
        };
    }
}

TravelingSalesmanEnvironment::TravelingSalesmanEnvironment() {
    locations.reserve( locationsCount );
    for ( size_t i = 0; i < locationsCount; ++i ) {
        locations.push_back( randomPoint());
    }
}

std::unique_ptr<aima::core::Percept> TravelingSalesmanEnvironment::getPerceptSeenBy( const core::Agent& agent ) {
    return std::unique_ptr<core::Percept>{ new TravelingSalesmanPercept{ locations }};
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

const Action TravelingSalesmanEnvironment::PLANNING{ "Planning" };
const Action TravelingSalesmanEnvironment::SUCCESS{ "Success" };
const Action TravelingSalesmanEnvironment::FAIL{ "Fail" };

#pragma clang diagnostic pop
