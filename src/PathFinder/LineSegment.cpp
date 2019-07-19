#include "LineSegment.hpp"
#include <iostream>

using namespace aima::path_finder;

LineSegmentIterator::LineSegmentIterator( const std::vector<Point>& vector, size_t index, bool wrap ) :
        vector{ &vector }, aIndex{ index }, wrap{ wrap } {
    if ( aIndex >= vector.size() || ( aIndex == vector.size() - 1 && !wrap ))
        this->vector = nullptr;
}

bool aima::path_finder::operator==( const LineSegmentIterator& a, const LineSegmentIterator& b ) {
    return ( a.vector == nullptr && b.vector == nullptr ) ||
           ( a.vector == b.vector && a.aIndex == b.aIndex );
}

const LineSegment LineSegmentIterator::operator*() const {
    return LineSegment{ vector->at( aIndex ),
                        vector->size() > aIndex + 1 ? vector->at( aIndex + 1 ) : *( vector->begin()) };
}

const LineSegment LineSegmentIterator::operator->() const {
    return operator*();
}

LineSegmentIterator& LineSegmentIterator::operator++() {
    if ( !vector ) return *this;

    ++aIndex;
    if ( aIndex == vector->size() || ( aIndex == vector->size() - 1 && !wrap )) vector = nullptr;

    return *this;
}

LineSegmentIterator LineSegmentIterator::operator++( int )& { // NOLINT(cert-dcl21-cpp)
    LineSegmentIterator ret{ *this };
    ++*this;
    return ret;
}

LineSegmentIterator& LineSegmentIterator::operator--() {
    if ( !vector ) return *this;

    if ( aIndex == 0 ) vector = nullptr;
    --aIndex;

    return *this;
}

LineSegmentIterator LineSegmentIterator::operator--( int )& { // NOLINT(cert-dcl21-cpp)
    LineSegmentIterator ret{ *this };
    --*this;
    return ret;
}
