#include "Point.hpp"
#include <iostream>
#include <algorithm>
#include <iterator>

std::ostream& aima::path_finder::operator<<( std::ostream& out, aima::path_finder::Point point ) {
    return out << '(' << point.x << ',' << point.y << ')';
}
