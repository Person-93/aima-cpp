#include "IterativeLengtheningAgent.hpp"
#include <utility>
#include "core/Action.hpp"
#include "core/Exception.hpp"
#include "util/define_logger.hpp"
#include "util/geometry/geometry.hpp"
#include "TravelingSalesmanPercept.hpp"

using namespace aima::traveling_salesman;

DEFINE_LOGGER( IterativeLengtheningAgent )

std::unique_ptr<aima::core::Action> IterativeLengtheningAgent::execute( const core::Percept& percept ) {
    LOG4CPLUS_DEBUG( GetLogger(), "Agent is searching..." );

    if ( firstCall ) {
        firstCall = false;
        auto p = dynamic_cast<const TravelingSalesmanPercept*>(&percept);
        if ( !p ) {
            using namespace aima::core::exception;
            AIMA_THROW_EXCEPTION( Exception{} << Because{ "Received percept of unrecognized type" } );
        }

        generator = iterativeLengthLimitedSearch( p->locations );
        iterator  = generator.begin();
    }
    else
        ++iterator;

    status.access( [ this ]( AgentStatus& status ) {
        status.pathLength = plan ? plan->pathCost : 0;
        ++status.timeSpent;
    } );

    switch ( *iterator ) {
        case SearchStatus::BUSY: return TravelingSalesmanEnvironment::PLANNING.clone();
        case SearchStatus::SUCCESS: {
            isAlive( false );
            return TravelingSalesmanEnvironment::SUCCESS.clone();
        }
        case SearchStatus::FAIL: {
            isAlive( false );
            return TravelingSalesmanEnvironment::FAIL.clone();
        }
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

IterativeLengtheningAgent::Generator
IterativeLengtheningAgent::iterativeLengthLimitedSearch( const TravelingSalesmanEnvironment::Locations& locations ) {
    std::vector<size_t> visited;
    visited.push_back( 0 );
    plan = makeNode( {}, std::move( visited ), 0 );

    float limit    = 0;
    float newLimit = std::numeric_limits<float>::infinity();
    while ( true ) {
        LOG4CPLUS_DEBUG( GetLogger(), "Starting new iteration. Length limit: " << limit );
        Generator  search = lengthLimitedSearch( locations, limit, newLimit );
        for ( auto result: search ) {
            switch ( result ) {
                case SearchStatus::SUCCESS: {
                    co_yield result;
                    co_return;
                }
                case SearchStatus::BUSY: {
                    co_yield result;
                    break;
                }
                case SearchStatus::FAIL: break;
            }
        }
        limit = newLimit;
    }
}

#pragma clang diagnostic pop

IterativeLengtheningAgent::Generator
IterativeLengtheningAgent::lengthLimitedSearch( const TravelingSalesmanEnvironment::Locations& locations,
                                                float limit,
                                                float& returnNewLimit ) {
    LOG4CPLUS_DEBUG( GetLogger(), "Length limited search at " << locations[ plan->visited.back() ]
                                                              << " length: " << plan->pathCost
                                                              << " limit: " << limit );
    LOG4CPLUS_DEBUG( GetLogger(), "Checking if succeeded" );
    if ( plan->visited.size() == TravelingSalesmanEnvironment::locationsCount ) {
        co_yield SearchStatus::SUCCESS;
        co_return;
    }

    std::vector<size_t> sorted{ plan->visited };
    std::sort( sorted.begin(), sorted.end());

    co_yield SearchStatus::BUSY;

    float        newLimit = std::numeric_limits<float>::infinity();
    for ( size_t i        = 0; i < locations.size(); ++i ) {
        if ( std::binary_search( sorted.begin(), sorted.end(), i )) continue;

        std::vector<size_t> visited{ plan->visited };
        visited.push_back( i );

        plan = makeNode( plan, std::move( visited ),
                         plan->pathCost +
                         util::geometry::distance( locations[ plan->visited.back() ], locations[ i ] ));

        co_yield SearchStatus::BUSY;

        if ( plan->pathCost <= limit ) {
            auto       search = lengthLimitedSearch( locations, limit, newLimit );
            for ( auto result: search ) {
                co_yield result;
                if ( result != SearchStatus::BUSY ) co_return;
            }
        }
        else {
            LOG4CPLUS_DEBUG( GetLogger(), "Length exceeded limit. Length: " << plan->pathCost << " Limit: " << limit );
            newLimit = std::min( newLimit, plan->pathCost );
            plan     = plan->parent;
            if ( !plan ) {
                AIMA_THROW_EXCEPTION( std::logic_error{ "null plan" } );
            }
        }
    }

    LOG4CPLUS_DEBUG( GetLogger(), "Did not find path with limit: " << limit );
    returnNewLimit = newLimit;
    co_yield SearchStatus::FAIL;
}

std::shared_ptr<SearchNode>
IterativeLengtheningAgent::makeNode( std::shared_ptr<SearchNode> parent, std::vector<size_t> visited, float cost ) {
    status.access( []( AgentStatus& s ) {
        ++s.nodesInMemory;
        s.maxNodesInMemory = std::max( s.maxNodesInMemory, s.nodesInMemory );
        ++s.nodesGenerated;
    } );
    return std::shared_ptr<SearchNode>{ new SearchNode{ std::move( parent ), std::move( visited ), cost },
                                        [ this ]( SearchNode* p ) {
                                            status.access( []( AgentStatus& s ) {
                                                --s.nodesInMemory;
                                            } );
                                            delete p;
                                        }};
}
