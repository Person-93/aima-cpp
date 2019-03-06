#pragma once

#include <map>
#include <functional>
#include <memory>
#include <boost/numeric/ublas/matrix.hpp>

#include "core/Environment.hpp"
#include "core/Agent.hpp"
#include "vacuum/types.hpp"

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
        using AgentLocations = std::map<std::reference_wrapper<const core::Agent>, Location, core::Agent::less>;
        using Locations = boost::numeric::ublas::matrix<LocationState>;

        static const core::Action ACTION_MOVE_LEFT;
        static const core::Action ACTION_MOVE_RIGHT;
        static const core::Action ACTION_MOVE_UP;
        static const core::Action ACTION_MOVE_DOWN;
        static const core::Action ACTION_SUCK;

        /**
         * Construct a vacuum environment with two locations named A and B and dirt is placed at random
         */
        explicit BasicVacuumEnvironment( unsigned x = 2, unsigned y = 1 );

        std::unique_ptr<core::Percept> getPerceptSeenBy( const core::Agent& agent ) override;

        Location getAgentLocation( const core::Agent& agent ) const;

        void setAgentLocation( const core::Agent& agent, Location location );

        void removeAgent( const core::Agent& agent ) override;

        bool isDone() const override;

        const Locations& getLocations() const;

        LocationState getLocationState( Location location ) const;

        void setLocationState( Location location, LocationState state );

        bool addAgent( core::Agent& agent ) override;

        void executeAction( const core::Agent& agent, const core::Action& action ) override;

        void createExogenousChange() override;

        unsigned getX() noexcept { return x; }

        unsigned getY() noexcept { return y; }

    protected:
        Location& getAgentLocationByRef( const core::Agent& agent );

        bool                  isDone_  = false;
        AgentLocations        agentLocations;
        static const unsigned maxSteps = 1000;

    private:
        const unsigned                x;
        const unsigned                y;
        std::function<unsigned int()> genXCoord;
        std::function<unsigned int()> genYCoord;
        Locations                     locations;
    };
}
