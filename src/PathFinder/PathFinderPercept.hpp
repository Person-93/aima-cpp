#pragma once

#include <vector>
#include "core/Percept.hpp"
#include "util/geometry/Point.hpp"
#include "PathFinderEnvironment.hpp"

namespace aima::path_finder {
    class PathFinderPercept : public core::Percept {
    public:
        std::unique_ptr<Percept> clone() const override;

        PathFinderPercept( const PathFinderEnvironment::Obstacles& obstacles,
                           util::geometry::Point location,
                           util::geometry::Point goal ) :
                obstacles{ obstacles }, location{ location }, goal{ goal } {}

        const PathFinderEnvironment::Obstacles& obstacles;
        const util::geometry::Point location;
        const util::geometry::Point goal;

    protected:
        void print( std::ostream& out ) const override;
    };
}
