#pragma once

#include <memory>
#include "core/App.hpp"
#include "vacuum/StatefulVacuumAgent.hpp"
#include "../vacuum/VacuumGui.hpp"

namespace aima::gui { struct ImGuiWrapper; }
namespace aima::core { class Environment; }
namespace aima::vacuum { class BasicVacuumEnvironment; }

namespace aima::apps {
    class StatefulVacuumApp : public core::App {
    public:
        explicit StatefulVacuumApp( gui::ImGuiWrapper& imGuiWrapper );

        std::shared_ptr<core::Environment>& environment() noexcept override;

        viewer::GraphicViewer& viewer() noexcept override;

    private:
        void renderImpl() override;

        vacuum::VacuumGui                  vacuumGui;
        std::shared_ptr<core::Environment> environment_;
        vacuum::StatefulVacuumAgent        agent;
    };

}
