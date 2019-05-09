#pragma once

#include <memory>
#include "core/App.hpp"
#include "vacuum/BasicVacuumAgent.hpp"
#include "../vacuum/VacuumGui.hpp"

// IWYU pragma: no_include "core/Environment.hpp"
// IWYU pragma: no_include "views/GraphicViewer.hpp"

namespace aima::gui { struct ImGuiWrapper; }
namespace aima::core { class Environment; } // IWYU pragma: keep
namespace aima::vacuum { class BasicVacuumEnvironment; }

namespace aima::apps {
    class TrivialVacuumApp : public core::App {
    public:
        explicit TrivialVacuumApp( gui::ImGuiWrapper& imGuiWrapper );

        std::shared_ptr<core::Environment>& environment() noexcept override;

        viewer::GraphicViewer& viewer() noexcept override;

    private:
        void renderImpl() override;

        vacuum::VacuumGui                  vacuumGui;
        std::shared_ptr<core::Environment> environment_;
        vacuum::BasicVacuumAgent           agent;
    };
}
