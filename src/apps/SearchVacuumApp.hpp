#pragma once

#include "core/App.hpp"
#include "core/AgentPicker.hpp"
#include "vacuum/VacuumGui.hpp"
#include "vacuum/SearchBasedAgent.hpp"

namespace aima::apps {
    class SearchVacuumApp final : public core::App {
    public:
        explicit SearchVacuumApp( gui::ImGuiWrapper& imGuiWrapper );

        std::shared_ptr<core::Environment>& environment() noexcept override { return env; }

        viewer::GraphicViewer& viewer() noexcept override { return viewer_; }

    private:
        void renderImpl() override;

    private:
        vacuum::VacuumGui                           viewer_;
        std::shared_ptr<core::Environment>          env;
        core::AgentPicker<vacuum::SearchBasedAgent> picker;
    };
}
