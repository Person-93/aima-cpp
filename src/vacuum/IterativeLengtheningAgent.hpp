#pragma once

#include "SearchBasedAgent.hpp"

namespace aima::vacuum {
    class IterativeLengtheningAgent final : public SearchBasedAgent {
    public:
        std::shared_ptr<const SearchNode> getPlan() const noexcept override { return plan; }

    protected:
        Generator search( GlobalVacuumEnvironmentPercept percept ) override;

    private:
        SearchBasedAgent::Generator lengthLimitedSearch( const std::vector<Location>& dirtySpots,
                                                         Location agentLocation,
                                                         float limit,
                                                         float& retNewLimit );

        std::shared_ptr<SearchNode> plan;
    };
}
