#pragma once

#include "views/GraphicViewer.hpp"
#include <string_view>
#include <unordered_map>
#include "PathFinderEnvironment.hpp"
#include "Point.hpp"

namespace aima::path_finder {
    class PathFinderGui final : public viewer::GraphicViewer {
    public:
        PathFinderGui( std::string_view title, bool* open, std::string_view str_id = {} );

        void setEnvironment( const std::shared_ptr<core::Environment>& environment ) override;

        void agentAdded( const core::Agent& agent, const core::Environment& source ) override;

    private:
        void initMethod( gui::ImGuiWrapper& imGuiWrapper ) override {}

        void renderDisplay( gui::ImGuiWrapper& imGuiWrapper, std::shared_ptr<core::Environment>& environment ) override;

        void renderInfo( const PathFinderEnvironment& env, gui::ImGuiWrapper& imGuiWrapper ) const;

        void renderPathArea( const aima::path_finder::PathFinderEnvironment& env, gui::ImGuiWrapper& imGuiWrapper );

        void renderObstacles( const PathFinderEnvironment& env ) const;

        void renderAgents( const PathFinderEnvironment& env ) const;

        void renderGoal( const PathFinderEnvironment& env ) const;

        void renderPlans( const PathFinderEnvironment& env ) const;

        gui::ChildWindowConfig                                                          childWindowConfig;
        std::unordered_map<PathFinderEnvironment::AgentRef, ImColor, core::Agent::hash> agentColors;
    };
}
