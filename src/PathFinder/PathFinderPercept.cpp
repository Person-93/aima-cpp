#include "PathFinderPercept.hpp"

using namespace aima::core;
using namespace aima::path_finder;

std::unique_ptr<Percept> PathFinderPercept::clone() const {
    return std::unique_ptr<Percept>{ new PathFinderPercept{ *this }};
}

void PathFinderPercept::print( std::ostream& out ) const {
    out << "Location: " << location << ", Goal: " << goal;
}
