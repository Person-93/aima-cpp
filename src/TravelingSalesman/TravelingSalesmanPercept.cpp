#include "TravelingSalesmanPercept.hpp"
#include <iostream>

void aima::traveling_salesman::TravelingSalesmanPercept::print( std::ostream& ostream ) const {
    ostream << "Locations: ";
    std::copy( locations.begin(), locations.end(), std::ostream_iterator<util::geometry::Point>{ ostream, "," } );
}
