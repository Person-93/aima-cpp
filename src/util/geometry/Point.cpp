#include "Point.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>

std::ostream& aima::util::geometry::operator<<( std::ostream& out, aima::util::geometry::Point point ) {
    return out << '(' << point.x << ',' << point.y << ')';
}
