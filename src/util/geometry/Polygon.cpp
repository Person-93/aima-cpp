#include "Polygon.hpp"
#include <algorithm>
#include <iterator>
#include "util/define_logger.hpp"
#include "core/Exception.hpp"
#include "util/geometry/geometry.hpp"

using namespace aima::util::geometry;

DEFINE_LOGGER( Polygon )

Polygon::Polygon( std::vector<Point>&& points ) : points{ std::move( points ) } {
    if ( this->points.size() < 3 ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << Because( "A polygon must have at least three points" ));
    }

    for ( LineSegmentIterator p = begin(); p != end(); ++p ) {
        for ( LineSegmentIterator q{ ++LineSegmentIterator{ p }}; q != end(); ++q ) {
            if ( p == q ) continue;
            if ( properIntersect( *p, *q )) {
                using namespace aima::core::exception;
                AIMA_THROW_EXCEPTION( Exception{} << Because( "Invalid Polygon" ));
            }
        }
    }
}

std::ostream& aima::util::geometry::operator<<( std::ostream& out, const Polygon& polygon ) {
    std::copy( polygon.points.begin(), polygon.points.end(), std::ostream_iterator<Point>{ out, "," } );
    return out;
}

std::ostream& aima::util::geometry::operator<<( std::ostream& out, const LineSegment& lineSegment ) {
    return out << '(' << lineSegment.a << ',' << lineSegment.b << ')';
}
