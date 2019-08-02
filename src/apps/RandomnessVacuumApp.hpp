#pragma once

#include "core/App.hpp"
#include "vacuum/VacuumGui.hpp"
#include "core/Environment.hpp"
#include "vacuum/RandomnessBasedAgent.hpp"

namespace aima::apps {
    class RandomnessVacuumApp final : public core::App {
    public:
        explicit RandomnessVacuumApp( gui::ImGuiWrapper& imGuiWrapper );

        std::shared_ptr<core::Environment>& environment() noexcept override { return env; }

        viewer::GraphicViewer& viewer() noexcept override { return viewer_; }

    private:
        void renderImpl() override;

        vacuum::VacuumGui                  viewer_;
        std::shared_ptr<core::Environment> env;
        vacuum::RandomnessBasedAgent       agent;
    };
}
