#pragma once

#include <vector>
#include <functional>
#include <random>
#include <boost/numeric/ublas/matrix.hpp>

#include "core/Environment.hpp"
#include "core/Action.hpp"
#include "types.hpp"

namespace aima::core {
    class Percept;
}

namespace aima::vacuum {
    namespace {
        using namespace aima::core;
    }

    /**
     * The world contains two locations. Each is either clean or dirty.
     * The agent can be in either location. The agent can move left, move right, or suck up dirt at its current location
     * The goal is to clean all the dirt
     */
    class BasicVacuumEnvironment : public Environment {
    public:
        using AgentLocations = std::map<std::reference_wrapper<const Agent>, Location, Agent::less>;
        using Locations = boost::numeric::ublas::matrix<LocationState>;

        static const Action ACTION_MOVE_LEFT;
        static const Action ACTION_MOVE_RIGHT;
        static const Action ACTION_MOVE_UP;
        static const Action ACTION_MOVE_DOWN;
        static const Action ACTION_SUCK;

        /**
         * Construct a vacuum environment with two locations named A and B and dirt is placed at random
         */
        explicit BasicVacuumEnvironment( unsigned x = 2, unsigned y = 1 );

        std::unique_ptr<Percept> getPerceptSeenBy( const Agent& agent ) override;

        Location getAgentLocation( const Agent& agent ) const;

        void setAgentLocation( const Agent& agent, Location location );

        void removeAgent( const Agent& agent ) override;

        bool isDone() const override;

        const Locations& getLocations() const;

        LocationState getLocationState( Location location ) const;

        void setLocationState( Location location, LocationState state );

        bool addAgent( Agent& agent ) override;

        void executeAction( const Agent& agent, const Action& action ) override;

        void createExogenousChange() override;

        unsigned getX() noexcept { return x; }

        unsigned getY() noexcept { return y; }

    protected:
        Location& getAgentLocationByRef( const Agent& agent );

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
