#pragma once

#include "core/App.hpp"
#include "PathFinder/PathFinderGui.hpp"
#include "PathFinder/PathFinderAgent.hpp"
#include "core/AgentPicker.hpp"

namespace aima::apps {
    class PathFinderApp : public core::App {
    public:
        explicit PathFinderApp( gui::ImGuiWrapper& imGuiWrapper );

        std::shared_ptr<core::Environment>& environment() noexcept override { return env; }

        viewer::GraphicViewer& viewer() noexcept override { return gui; }

    private:
        void renderImpl() override;

        path_finder::PathFinderGui                      gui;
        std::shared_ptr<core::Environment>              env;
        core::AgentPicker<path_finder::PathFinderAgent> agentPicker;
    };
}
