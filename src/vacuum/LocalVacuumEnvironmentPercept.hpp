#pragma once

#include <iosfwd>
#include <memory>
#include "core/Percept.hpp" // IWYU pragma: export
#include "misc.hpp"

namespace aima::vacuum {
    class LocalVacuumEnvironmentPercept : public core::Percept {
    public:
        LocalVacuumEnvironmentPercept( const Location& location, LocationState locationState );

        LocalVacuumEnvironmentPercept& operator=( const LocalVacuumEnvironmentPercept& ) = delete;

        LocalVacuumEnvironmentPercept& operator=( LocalVacuumEnvironmentPercept&& ) = delete;

        std::unique_ptr<Percept> clone() const override;

        const Location      agentLocation;
        const LocationState agentLocationState;

    protected:
        LocalVacuumEnvironmentPercept( const LocalVacuumEnvironmentPercept& ) = default;

        LocalVacuumEnvironmentPercept( LocalVacuumEnvironmentPercept&& ) = default;

        void print( std::ostream& out ) const override;
    };
}
