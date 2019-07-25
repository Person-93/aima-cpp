#pragma once

#include <optional>
#include <iterator>
#include <vector>
#include "Point.hpp"
#include "LineSegment.hpp"

namespace aima::util::geometry {
    class Polygon;

    std::optional<Point> lineIntersect( const LineSegment& a, const LineSegment& b );

    std::optional<Point> segmentIntersect( const LineSegment& p, const LineSegment& q );

    std::optional<Point> properIntersect( const LineSegment& p, const LineSegment& q );

    struct Triangle { Point a, b, c; };

    bool isOnSegment( const Point& point, const LineSegment& segment );

    float distance( const Point& a, const Point& b );

    /**
         * Returns true if the line segment formed by the two points cuts the polygon
         * @param a
         * @param b
         * @return
         */
    bool isCutBy( const Polygon& polygon, const LineSegment& lineSegment );
}
