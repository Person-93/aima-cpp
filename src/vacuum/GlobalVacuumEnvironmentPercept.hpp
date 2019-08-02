#pragma once

#include "LocalVacuumEnvironmentPercept.hpp"
#include "BasicVacuumEnvironment.hpp"

namespace aima::vacuum {
    class GlobalVacuumEnvironmentPercept final : public LocalVacuumEnvironmentPercept {
    public:
        explicit GlobalVacuumEnvironmentPercept( const Location& agentLocation,
                                                 const BasicVacuumEnvironment::Locations& locations ) :
                LocalVacuumEnvironmentPercept{ agentLocation, locations( agentLocation.x, agentLocation.y ) },
                locations{ locations } {}

        std::unique_ptr<Percept> clone() const override {
            return std::unique_ptr<Percept>{ new GlobalVacuumEnvironmentPercept{ *this }};
        }

        const BasicVacuumEnvironment::Locations locations;
    protected:
        void print( std::ostream& ostream ) const override;
    };
}
