#pragma once

#include "util/App.hpp"
#include "BasicVacuumEnvironment.hpp"
#include "BasicVacuumAgent.hpp"
#include "VacuumGui.hpp"

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
