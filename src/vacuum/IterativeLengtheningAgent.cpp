#include "IterativeLengtheningAgent.hpp"
#include <utility>
#include <sstream>
#include "util/define_logger.hpp"
#include "core/registerAgent.hpp"
#include "core/Exception.hpp"

using namespace aima::vacuum;

DEFINE_LOGGER( IterativeLengtheningAgent )

AIMA_REGISTER_AGENT( SearchBasedAgent, IterativeLengtheningAgent ); // NOLINT(cert-err58-cpp)

namespace {
    /**
     * Cost is manhattan distance times the amount of dirty spots
     * @param currentLocation
     * @param destination
     * @param dirtyCount
     * @return
     */
    unsigned long cost( const Location& currentLocation, const Location& destination, unsigned long dirtyCount ) {
        auto distance = abs((long long) currentLocation.x - (long long) destination.x ) +
                        abs((long long) currentLocation.y - (long long) destination.y );
        return distance * dirtyCount;
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

SearchBasedAgent::Generator IterativeLengtheningAgent::search( GlobalVacuumEnvironmentPercept percept ) {
    plan                    = std::make_shared<SearchNode>( std::shared_ptr<SearchNode>{}, percept.agentLocation, 0 );

    std::vector<Location> dirtySpots;
    for ( auto            i = percept.locations.cbegin1(); i != percept.locations.cend1(); ++i )
        for ( auto j = i.begin(); j != i.end(); ++j )
            if ( percept.locations( j.index1(), j.index2()) == LocationState::DIRTY )
                dirtySpots.push_back( { j.index1(), j.index2() } );


    float limit    = 0;
    float newLimit = std::numeric_limits<float>::infinity();
    while ( true ) {
        LOG4CPLUS_DEBUG( GetLogger(), "Starting new iteration. Limit: " << limit );
        Generator  gen = lengthLimitedSearch( dirtySpots, percept.agentLocation, limit, newLimit );
        for ( auto result: gen ) {
            switch ( result ) {
                case SearchStatus::SUCCESS: {
                    co_yield result;
                    co_return;
                }
                case SearchStatus::BUSY: [[fallthrough]];
                case SearchStatus::FAIL: {
                    co_yield SearchStatus::BUSY;
                    break;
                }
            }
        }
        limit = newLimit;
    }
}

#pragma clang diagnostic pop

SearchBasedAgent::Generator IterativeLengtheningAgent::lengthLimitedSearch( const std::vector<Location>& dirtySpots,
                                                                            const Location agentLocation,
                                                                            float limit,
                                                                            float& retNewLimit ) {
    LOG4CPLUS_DEBUG( GetLogger(), "Length limited search. Limit: " << limit
                                                                   << " Location: " << agentLocation
                                                                   << " Length: " << plan->pathCost );
    LOG4CPLUS_DEBUG( GetLogger(), "Checking if succeeded" );
    if ( dirtySpots.empty()) {
        co_yield SearchStatus::SUCCESS;
        co_return;
    }

    {
        std::ostringstream ss;
        ss << "Dirty spots remaining: ";
        std::copy( dirtySpots.begin(), dirtySpots.end(), std::ostream_iterator<Location>{ ss } );
        LOG4CPLUS_DEBUG( GetLogger(), ss.str());
    }

    float        newLimit = std::numeric_limits<float>::infinity();
    for ( size_t i        = 0; i < dirtySpots.size(); ++i ) {
        const auto dirtySpot = dirtySpots[ i ];

        {
            auto newPlan = std::make_shared<SearchNode>( plan, dirtySpot, plan->pathCost +
                                                                          cost( agentLocation, dirtySpot,
                                                                                dirtySpots.size()));
            plan = std::move( newPlan );
        }

        co_yield SearchStatus::BUSY;

        {
            std::ostringstream ss;
            for ( auto         temp = plan; temp; temp = temp->parent )
                ss << temp->location;
            LOG4CPLUS_DEBUG( GetLogger(), "Checking plan " << ss.str());
        }

        if ( plan->pathCost <= limit ) {
            std::vector<Location> remainingDirtySpots{ dirtySpots };
            remainingDirtySpots.erase( remainingDirtySpots.begin() + i );
            auto       gen = lengthLimitedSearch( remainingDirtySpots, dirtySpot, limit, newLimit );
            for ( auto result: gen ) {
                if ( result == SearchStatus::FAIL ) {
                    plan = plan->parent;
                    if ( !plan )
                        AIMA_THROW_EXCEPTION( std::logic_error{ "null plan" } );
                    break;
                }
                co_yield result;
                if ( result == SearchStatus::SUCCESS ) co_return;
            }
        }
        else {
            LOG4CPLUS_DEBUG( GetLogger(), "Length exceeded limit. Length: " << plan->pathCost << " Limit: " << limit );
            newLimit = std::min( newLimit, plan->pathCost );
            plan     = plan->parent;
            if ( !plan )
                AIMA_THROW_EXCEPTION( std::logic_error{ "null plan" } );

        }
    }

    retNewLimit = newLimit;
    co_yield SearchStatus::FAIL;
}
