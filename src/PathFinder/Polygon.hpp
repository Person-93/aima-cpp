#pragma once

#include <vector>
#include <iosfwd>
#include "Point.hpp"
#include "LineSegment.hpp"

namespace aima::path_finder {

    class Polygon {
    public:
        explicit Polygon( std::vector<Point>&& points );

        Polygon( const Polygon& ) = default;

        Polygon( Polygon&& ) = default;

        Polygon& operator=( const Polygon& ) = default;

        Polygon& operator=( Polygon&& ) = default;



        LineSegmentIterator begin() const {
            return LineSegmentIterator{ points, 0 };
        }

        LineSegmentIterator end() const {
            return LineSegmentIterator{};
        }

        const std::vector<Point>& getPoints() const { return points; };

        friend std::ostream& operator<<( std::ostream& out, const Polygon& polygon );

    private:
        std::vector<Point> points;
    };

    std::ostream& operator<<( std::ostream& out, const Polygon& polygon );
}
