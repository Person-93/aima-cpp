#pragma once

#include <unordered_map>
#include <functional>
#include <memory>
#include <boost/numeric/ublas/matrix.hpp>
#include <vector>
#include "core/Environment.hpp" // IWYU pragma: export
#include "core/Agent.hpp"
#include "misc.hpp"

// IWYU pragma: no_include <boost/numeric/ublas/functional.hpp>
// IWYU pragma: no_include <boost/numeric/ublas/storage.hpp>
// IWYU pragma: no_include "util/UniqueIdMixin.hpp"

namespace aima::core {
    class Percept;

    class Action;
}

namespace aima::vacuum {
    /**
     * The world contains two locations. Each is either clean or dirty.
     * The agent can be in either location. The agent can move left, move right, or suck up dirt at its current location
     * The goal is to clean all the dirt
     */
    class BasicVacuumEnvironment : public core::Environment {
    public:
        using AgentLocations = std::unordered_map<std::reference_wrapper<const core::Agent>,
                                                  Location,
                                                  core::Agent::hash>;
        using Locations = boost::numeric::ublas::matrix<LocationState>;

        static const core::Action ACTION_MOVE_LEFT;
        static const core::Action ACTION_MOVE_RIGHT;
        static const core::Action ACTION_MOVE_UP;
        static const core::Action ACTION_MOVE_DOWN;
        static const core::Action ACTION_SUCK;

        /**
         * Construct a vacuum environment with two locations named A and B and dirt is placed at random
         */
        explicit BasicVacuumEnvironment( unsigned long x = 2, unsigned long y = 1 );

        std::unique_ptr<core::Environment> clone() override;

        std::unique_ptr<core::Percept> getPerceptSeenBy( const core::Agent& agent ) override;

        const Location& getAgentLocation( const core::Agent& agent ) const { return agentLocations.at( agent ); }

        const AgentLocations& getAgentLocations() const noexcept { return agentLocations; }

        std::vector<Location> getAgentLocationsList() const;

        void setAgentLocation( const core::Agent& agent, const Location& location );

        void removeAgent( const core::Agent& agent ) override;

        bool isDone() const override;

        const Locations& getLocations() const noexcept { return locations; }

        LocationState getLocationState( const Location& location ) const;

        void setLocationState( const Location& location, LocationState state );

        bool addAgent( core::Agent& agent ) override;

        void executeAction( const core::Agent& agent, const core::Action& action ) override;

        void createExogenousChange() override;

        unsigned long getX() const { return locations.size1(); }

        unsigned long getY() const { return locations.size2(); }

    protected:
        BasicVacuumEnvironment( const BasicVacuumEnvironment& ) = default;

        Location& getAgentLocationByRef( const core::Agent& agent ) { return agentLocations[ agent ]; }

        Location randomLocation() const;

        constexpr unsigned maxSteps() const noexcept { return 1000; }

        bool agentStopped() const noexcept { return agentStopped_; }

        void agentStopped( bool stopped ) { agentStopped_ = stopped; }

    private:
        bool           agentStopped_ = false;
        Locations      locations;
        AgentLocations agentLocations;
    };
}
