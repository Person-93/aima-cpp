#pragma once

#include "core/Agent.hpp"
#include <cppcoro/recursive_generator.hpp>
#include "GlobalVacuumEnvironmentPercept.hpp"

namespace aima::vacuum {
    struct SearchNode {
        SearchNode( std::shared_ptr<SearchNode> parent, Location location, float pathCost ) :
                parent{ std::move( parent ) }, location{ location }, pathCost{ pathCost } {}

        Location                    location;
        std::shared_ptr<SearchNode> parent;
        float                       pathCost;
    };

    class SearchBasedAgent : public core::Agent {
    public:
        std::unique_ptr<Agent> clone() const override {
            return std::unique_ptr<Agent>();
        }

        std::unique_ptr<core::Action> execute( const core::Percept& percept ) override;

        virtual std::shared_ptr<const SearchNode> getPlan() const noexcept = 0;

        std::optional<Location> nextStop() const {
            return itineraryIter == itinerary.crend() ? std::nullopt : std::optional{ *itineraryIter };
        }

    protected:
        enum class SearchStatus { BUSY, SUCCESS, FAIL };

        using Generator = cppcoro::recursive_generator<SearchStatus>;

        virtual Generator search( GlobalVacuumEnvironmentPercept percept ) = 0;

    private:
        std::unique_ptr<core::Action> executePlan( const Location& agentLocation );

        void setDestination( const Location& agentLocation );

        std::unique_ptr<core::Action> continuePlanning( const GlobalVacuumEnvironmentPercept& p );

        Generator                                     generator;
        Generator::iterator                           iterator;
        std::vector<Location>                         itinerary;
        bool                                          firstCall{ true };
        bool                                          madePlan{ false };
        bool                                          startedExecuting{ false };
        std::vector<Location>::const_reverse_iterator itineraryIter{};
    };
}
