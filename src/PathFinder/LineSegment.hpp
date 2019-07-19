#pragma once

#include <iosfwd>
#include "Point.hpp"

namespace aima::path_finder {
    struct LineSegment { Point a, b; };

    std::ostream& operator<<( std::ostream& out, const LineSegment& lineSegment );

    class LineSegmentIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = const LineSegment;
        using difference_type   = ptrdiff_t;
        using reference         = const LineSegment&;
        using pointer           = void;

        LineSegmentIterator() = default;

        LineSegmentIterator( const LineSegmentIterator& ) = default;

        LineSegmentIterator( LineSegmentIterator&& ) = default;

        LineSegmentIterator& operator=( const LineSegmentIterator& ) = default;

        LineSegmentIterator& operator=( LineSegmentIterator&& ) = default;

        LineSegmentIterator( const std::vector<Point>& vector, size_t index, bool wrap = true );

        friend bool operator==( const LineSegmentIterator& a, const LineSegmentIterator& b );

        friend bool operator!=( const LineSegmentIterator& a, const LineSegmentIterator& b );

        const LineSegment operator*() const;

        const LineSegment operator->() const;

        LineSegmentIterator& operator++();

        LineSegmentIterator operator++( int )&; // NOLINT(cert-dcl21-cpp)

        LineSegmentIterator& operator--();

        LineSegmentIterator operator--( int )&; // NOLINT(cert-dcl21-cpp)

    private:
        const std::vector<Point>* vector{};
        size_t aIndex{};
        bool   wrap{ true };
    };

    bool operator==( const LineSegmentIterator& a, const LineSegmentIterator& b );

    inline bool operator!=( const LineSegmentIterator& a, const LineSegmentIterator& b ) { return !( a == b ); }
}

