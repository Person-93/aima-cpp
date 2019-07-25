#pragma once

#include "core/Environment.hpp"
#include <vector>
#include <functional>
#include "util/geometry/Point.hpp"
#include "util/geometry/Polygon.hpp"

namespace aima::core {
    class Agent;

    class Action;

    class Percept;
}

namespace aima::path_finder {
    class PathFinderEnvironment : public core::Environment {
    public:
        using AgentRef       = std::reference_wrapper<const core::Agent>;
        using AgentLocations = std::unordered_map<AgentRef, util::geometry::Point, core::Agent::hash>;
        using Obstacles      = std::vector<util::geometry::Polygon>;

        PathFinderEnvironment();

        static const core::Action PLANNING;
        static const core::Action SUCCEEDED;
        static const core::Action FAILED;

        std::unique_ptr<core::Environment> clone() override;

        bool addAgent( core::Agent& agent ) override;

        void removeAgent( const core::Agent& agent ) override;

        void executeAction( const core::Agent& agent, const core::Action& action ) override {}

        std::unique_ptr<core::Percept> getPerceptSeenBy( const core::Agent& agent ) override;

        util::geometry::Point getAgentLocation( const core::Agent& agent ) const;

        const AgentLocations& getAgentLocations() const { return agentLocations; }

        const Obstacles& getObstacles() const { return obstacles; }

        const util::geometry::Point& getGoal() const { return goal; }

    private:
        Obstacles             obstacles;
        AgentLocations        agentLocations;
        util::geometry::Point goal;
    };
}
