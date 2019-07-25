#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <optional>
#include "util/geometry/geometry.hpp"
#include "util/geometry/Polygon.hpp"

using aima::util::geometry::LineSegment;
using aima::util::geometry::Point;
using aima::util::geometry::Polygon;
using namespace boost::unit_test::data;
using boost::unit_test_framework::depends_on;

namespace aima::util::geometry {
    std::ostream& boost_test_print_type( std::ostream& out, const std::optional<Point>& point ) {
        if ( point ) {
            out << *point;
        }
        else {
            out << "()";
        }
        return out;
    }
}

BOOST_AUTO_TEST_SUITE( geometry )

    std::tuple<Point, LineSegment, bool> isOnSegmentData[]{
            {
                    Point{ 1, 1 },
                    LineSegment{{ 0, 0 },
                                { 2, 2 }},
                    true
            },
            {
                    Point{ 0, 0 },
                    LineSegment{{ 0, 0 },
                                { 0, 1 }},
                    true
            },
            {
                    Point{ 0, 0 },
                    LineSegment{{ 1, 1 },
                                { 2, 2 }},
                    false
            }
    };

    BOOST_DATA_TEST_CASE( isOnSegment, make( isOnSegmentData ), point, segment, expected ) {
        BOOST_CHECK_EQUAL( aima::util::geometry::isOnSegment( point, segment ), expected );
    }


    using IntersectTestData = std::tuple<LineSegment, LineSegment, std::optional<Point>>[];

    IntersectTestData lineIntersectData{
            {
                    LineSegment{{ 0,  0 },
                                { .5, .5 }},
                    LineSegment{{ 0, 2 },
                                { 2, 0 }},
                    Point{ 1, 1 }
            },
            {
                    LineSegment{{ 1, 0 },
                                { 3, 2 }},
                    LineSegment{{ 1, 2 },
                                { 3, 0 }},
                    Point{ 2, 1 }
            },
            {
                    LineSegment{{ 0, 0 },
                                { 0, 1 }},
                    LineSegment{{ 1, 0 },
                                { 1, 1 }},
                    std::nullopt
            },
            {
                    LineSegment{{ 0, 0 },
                                { 0, 1 }},
                    LineSegment{{ 0, 3 },
                                { 0, 2 }},
                    std::nullopt
            }
    };

    BOOST_DATA_TEST_CASE( lineIntersect, make( lineIntersectData ), p, q, expected ) {
        BOOST_CHECK_EQUAL( aima::util::geometry::lineIntersect( p, q ), expected );
    }


    IntersectTestData segmentIntersectData{
            {
                    LineSegment{{ 0,  0 },
                                { .5, .5 }},
                    LineSegment{{ 0, 2 },
                                { 2, 0 }},
                    std::nullopt
            },
            {
                    LineSegment{{ 0, 1 },
                                { 1, 1 }},
                    LineSegment{{ 0, 2 },
                                { 1, 0 }},
                    Point{ .5, 1 },
            }
    };

    BOOST_TEST_DECORATOR( *depends_on( "geometry/lineIntersect" ))
    BOOST_TEST_DECORATOR( *depends_on( "geometry/isOnSegment" ))

    BOOST_DATA_TEST_CASE( segmentIntersect, make( segmentIntersectData ), p, q, expected ) {
        BOOST_CHECK_EQUAL( aima::util::geometry::segmentIntersect( p, q ), expected );
    }


    std::tuple<Polygon, LineSegment, bool> isCutByData[]{
            {
                    Polygon{{{ 1, 1 }, { 1, 0 }, { 0, 0 }, { 0, 1 }}},
                    LineSegment{{ 1, 0 },
                                { 0, 2 }},
                    true
            }
    };

    BOOST_TEST_DECORATOR( *depends_on( "geometry/segmentIntersect" ))

    BOOST_DATA_TEST_CASE( isCutBy, make( isCutByData ), polygon, lineSegment, expected ) {
        BOOST_CHECK_EQUAL( aima::util::geometry::isCutBy( polygon, lineSegment ), expected );
    }

BOOST_AUTO_TEST_SUITE_END() // NOLINT(cert-err58-cpp)

#pragma clang diagnostic pop