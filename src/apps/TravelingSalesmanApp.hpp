#pragma once

#include "core/App.hpp"
#include "TravelingSalesman/TravelingSalesmanGui.hpp"
#include "core/Environment.hpp"
#include "TravelingSalesman/IterativeLengtheningAgent.hpp"

namespace aima::apps {
    class TravelingSalesmanApp final : public core::App {
    public:
        TravelingSalesmanApp( gui::ImGuiWrapper& imGuiWrapper );

        std::shared_ptr<core::Environment>& environment() noexcept override { return env; }

        viewer::GraphicViewer& viewer() noexcept override { return viewer_; }

    private:
        void renderImpl() override;

        traveling_salesman::TravelingSalesmanGui      viewer_;
        std::shared_ptr<core::Environment>            env;
        traveling_salesman::IterativeLengtheningAgent agent;
    };
}
