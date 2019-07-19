#pragma once

#include <vector>
#include "core/Percept.hpp"
#include "Point.hpp"
#include "PathFinderEnvironment.hpp"

namespace aima::path_finder {
    class Obstacle;

    class PathFinderPercept : public core::Percept {
    public:
        std::unique_ptr<Percept> clone() const override;

        PathFinderPercept( const PathFinderEnvironment::Obstacles& obstacles,
                           Point location,
                           Point goal ) :
                obstacles{ obstacles }, location{ location }, goal{ goal } {}

        const PathFinderEnvironment::Obstacles& obstacles;
        const Point location;
        const Point goal;

    protected:
        void print( std::ostream& out ) const override;
    };
}
