#include "geometry.hpp"
#include <algorithm>
#include <cmath>
#include "Polygon.hpp"

using namespace aima::util::geometry;

static float determinant( float a, float b, float c, float d ) {
    return a * d - b * c;
}

std::optional<Point> aima::util::geometry::lineIntersect( const LineSegment& p, const LineSegment& q ) {
    float detP = determinant( p.a.x, p.a.y, p.b.x, p.b.y );
    float detQ = determinant( q.a.x, q.a.y, q.b.x, q.b.y );

    float xDifP = p.a.x - p.b.x;
    float xDifQ = q.a.x - q.b.x;

    float yDifP = p.a.y - p.b.y;
    float yDifQ = q.a.y - q.b.y;

    float denominator = determinant( xDifP, yDifP, xDifQ, yDifQ );

    if ( denominator == 0 )
        return std::nullopt;

    float xNom = determinant( detP, xDifP, detQ, xDifQ );
    float yNom = determinant( detP, yDifP, detQ, yDifQ );

    return Point{ xNom / denominator, yNom / denominator };
}

std::optional<Point> aima::util::geometry::segmentIntersect( const LineSegment& p, const LineSegment& q ) {
    auto intersection = lineIntersect( p, q );
    if ( !intersection ) return intersection;

    if ( !isOnSegment( *intersection, p ) || !isOnSegment( *intersection, q ))
        intersection.reset();

    return intersection;
}

std::optional<Point> aima::util::geometry::properIntersect( const LineSegment& p, const LineSegment& q ) {
    auto intersection = segmentIntersect( p, q );
    if ( !intersection ) return intersection;

    if ( *intersection == p.a || *intersection == p.b || *intersection == q.a || *intersection == q.b )
        intersection.reset();

    return intersection;
}

bool aima::util::geometry::isOnSegment( const Point& point, const LineSegment& segment ) {
    auto[xMin, xMax] = std::minmax( { segment.a.x, segment.b.x } );
    auto[yMin, yMax] = std::minmax( { segment.a.y, segment.b.y } );
    return
            point.x >= xMin && point.x <= xMax &&
            point.y >= yMin && point.y <= yMax;
}

float aima::util::geometry::distance( const Point& a, const Point& b ) {
    return static_cast<float>(std::sqrt( std::pow( a.x - b.x, 2 ) + std::pow( a.y - b.y, 2 )));
}

bool aima::util::geometry::isCutBy( const Polygon& polygon, const LineSegment& lineSegment ) {
    auto a = std::find( polygon.getPoints().begin(), polygon.getPoints().end(), lineSegment.a );
    auto b = std::find( polygon.getPoints().begin(), polygon.getPoints().end(), lineSegment.b );

    // if both end of the line segments are on the polygon, then the segment either runs along it,
    // in which case the distance between the iterators would be 1 and the segment does not cut the polygon
    if ( a != polygon.getPoints().end() && b != polygon.getPoints().end()) {
        if ( std::abs( std::distance( a, b )) > 1 ) return true;
    }


    return std::any_of( polygon.begin(), polygon.end(),
                        [ &lineSegment ]( const LineSegment& q ) {
                            return properIntersect( lineSegment, q ).has_value();
                        } );
}
