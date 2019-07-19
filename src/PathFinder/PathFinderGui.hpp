#pragma once

#include "views/GraphicViewer.hpp"
#include <string_view>
#include "Point.hpp"

namespace aima::path_finder {
    class PathFinderEnvironment;

    struct Triangle { Point a, b, c; };

    class PathFinderGui final : public viewer::GraphicViewer {
    public:
        PathFinderGui( std::string_view title, bool* open, std::string_view str_id = {} );

        void setEnvironment( const std::shared_ptr<core::Environment>& environment ) override;

    private:
        void initMethod( gui::ImGuiWrapper& imGuiWrapper ) override {}

        void renderDisplay( gui::ImGuiWrapper& imGuiWrapper, std::shared_ptr<core::Environment>& environment ) override;

        void renderObstacles( const PathFinderEnvironment& env ) const;

        void renderAgents( const PathFinderEnvironment& env ) const;

        void renderGoal( const PathFinderEnvironment& env ) const;

        void renderPlans( const PathFinderEnvironment& env ) const;

        gui::ChildWindowConfig childWindowConfig;
    };
}
