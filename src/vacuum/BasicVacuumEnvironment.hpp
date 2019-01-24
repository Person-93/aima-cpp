#pragma once

#include <vector>
#include <functional>
#include <random>

#include <core/Environment.hpp>
#include <core/Action.hpp>

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
    class BasicVacuumEnvironmentBase : public Environment {
    public:
        using AgentLocations = std::map<std::reference_wrapper<const Agent>, Location, Agent::less>;

        static const Action ACTION_MOVE_LEFT;
        static const Action ACTION_MOVE_RIGHT;
        static const Action ACTION_MOVE_UP;
        static const Action ACTION_MOVE_DOWN;
        static const Action ACTION_SUCK;

        std::unique_ptr<Percept> getPerceptSeenBy( const Agent& agent ) override;

        Location getAgentLocation( const Agent& agent ) const;

        void setAgentLocation( const Agent& agent, Location location );

        virtual void removeAgent( const Agent& agent ) override;

        virtual LocationState getLocationState( Location location ) const = 0;

        virtual void setLocationState( Location location, LocationState state ) = 0;

        bool isDone() const override;

    protected:
        Location& getAgentLocationByRef( const Agent& agent );

        bool                  isDone_  = false;
        AgentLocations        agentLocations;
        static const unsigned maxSteps = 1000;
    };

    template< unsigned int X = 2, unsigned int Y = 1 >
    class BasicVacuumEnvironment : public BasicVacuumEnvironmentBase {
    public:
        using Locations = std::array<std::array<LocationState, Y>, X>;

        /**
         * Construct a vacuum environment with two locations named A and B and dirt is placed at random
         */
        BasicVacuumEnvironment() {
            using std::bind;
            using std::mt19937;
            using std::uniform_int_distribution;
            std::random_device rd{};
            static auto        randomBool = bind( uniform_int_distribution( 0, 1 ), // NOLINT(cert-err58-cpp)
                                                  mt19937( rd()));

            for ( unsigned int i = 0; i < X; ++i )
                for ( unsigned int j = 0; j < Y; ++j )
                    locations[ i ][ j ] = randomBool() ? LocationState::CLEAN : LocationState::DIRTY;

            genXCoord = bind( uniform_int_distribution<>( 0, X - 1 ), mt19937( rd()));
            genYCoord = bind( uniform_int_distribution<>( 0, Y - 1 ), mt19937( rd()));
        }

        const Locations& getLocations() const {
            return locations;
        }

        LocationState getLocationState( Location location ) const override {
            return locations.at( location.x ).at( location.y );
        }

        void setLocationState( Location location, LocationState state ) override {
            locations.at( location.x ).at( location.y ) = state;
        }

        virtual bool addAgent( Agent& agent ) override {
            auto isNew = Environment::addAgent( agent );
            if ( isNew ) agentLocations[ agent ] = Location{ genXCoord(), genYCoord() };
            return isNew;
        }

        void executeAction( const Agent& agent, const Action& action ) override {
            auto& location = getAgentLocationByRef( agent );

            if ( action == ACTION_MOVE_DOWN ) {
                if ( location.y < Y - 1 ) {
                    ++location.y;
                    updatePerformanceMeasure( agent, -1 );
                }
            }
            else if ( action == ACTION_MOVE_UP ) {
                if ( location.y > 0 ) {
                    --location.y;
                    updatePerformanceMeasure( agent, -1 );
                }
            }
            else if ( action == ACTION_MOVE_RIGHT ) {
                if ( location.x < X - 1 ) {
                    ++location.x;
                    updatePerformanceMeasure( agent, -1 );
                }
            }
            else if ( action == ACTION_MOVE_LEFT ) {
                if ( location.x > 0 ) {
                    --location.x;
                    updatePerformanceMeasure( agent, -1 );
                }
            }
            else if ( action == ACTION_SUCK ) { locations.at( location.x ).at( location.y ) = LocationState::CLEAN; }
            else if ( action == Action::noOp()) { isDone_ = true; }
            else throw std::out_of_range( "Unrecognized action received" );
        }

        void createExogenousChange() override {
            int amount = isDone() ? maxSteps - stepCount : 1;
            ++stepCount;
            for ( unsigned i = 0; i < X; ++i )
                for ( unsigned j = 0; j < Y; ++j )
                    if ( locations[ i ][ j ] == LocationState::CLEAN )
                        for ( Agent& agent: getAgents())
                            updatePerformanceMeasure( agent, amount );
        }

        static constexpr unsigned getX() noexcept { return X; }

        static constexpr unsigned getY() noexcept { return Y; }

    private:
        std::function<unsigned int()> genXCoord;
        std::function<unsigned int()> genYCoord;
        Locations                     locations;
    };
}
