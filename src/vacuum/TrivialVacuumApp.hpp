#pragma once

#include <memory>

#include "util/App.hpp"
#include "vacuum/BasicVacuumEnvironment.hpp"
#include "vacuum/BasicVacuumAgent.hpp"
#include "vacuum/VacuumGui.hpp"

namespace aima::gui { struct ImGuiWrapper; }

namespace aima::vacuum {
    class TrivialVacuumApp : public util::App {
    public:
        explicit TrivialVacuumApp( gui::ImGuiWrapper& imGuiWrapper, bool* open );

        void render() override;

    private:
        std::shared_ptr<BasicVacuumEnvironment> environment = std::make_shared<BasicVacuumEnvironment>();
        BasicVacuumAgent                        agent;
        VacuumGui                               vacuumGui;
    };
}
