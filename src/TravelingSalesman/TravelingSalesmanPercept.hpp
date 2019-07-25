#pragma once

#include "core/Percept.hpp"
#include "util/geometry/Point.hpp"
#include "TravelingSalesmanEnvironment.hpp"

namespace aima::traveling_salesman {
    class TravelingSalesmanPercept final : public core::Percept {
    public:
        explicit TravelingSalesmanPercept( const TravelingSalesmanEnvironment::Locations& locations ) :
                locations{ locations } {}

        std::unique_ptr<Percept> clone() const override {
            return std::unique_ptr<Percept>{ new TravelingSalesmanPercept{ *this }};
        }

    protected:
        void print( std::ostream& ostream ) const override;

    public:
        const TravelingSalesmanEnvironment::Locations& locations;
    };
}
