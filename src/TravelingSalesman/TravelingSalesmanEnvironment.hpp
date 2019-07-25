#pragma once

#include "core/Environment.hpp"
#include <vector>
#include "util/geometry/Point.hpp"

namespace aima::traveling_salesman {
    class TravelingSalesmanEnvironment final : public core::Environment {
    public:
        using Locations = std::vector<util::geometry::Point>;

        TravelingSalesmanEnvironment();

        TravelingSalesmanEnvironment( const TravelingSalesmanEnvironment& ) = default;

        std::unique_ptr<Environment> clone() override {
            return std::unique_ptr<Environment>{ new TravelingSalesmanEnvironment{ *this }};
        }

        void executeAction( const core::Agent& agent, const core::Action& action ) override {}

        std::unique_ptr<core::Percept> getPerceptSeenBy( const core::Agent& agent ) override;

        const Locations& getLocations() const noexcept { return locations; }

        static const core::Action PLANNING;
        static const core::Action SUCCESS;
        static const core::Action FAIL;

        static constexpr size_t locationsCount = 20;

    private:
        Locations locations{};
    };
}
