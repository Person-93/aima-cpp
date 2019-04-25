#include "TrivialVacuumApp.hpp"
#include "../vacuum/BasicVacuumEnvironment.hpp"
#include "util/parseTitle.hpp"
#include "util/define_logger.hpp"


namespace aima::gui { class ImGuiWrapper; }

using namespace aima::gui;
using namespace aima::vacuum;
using namespace aima::util;
using aima::apps::TrivialVacuumApp;

DEFINE_LOGGER( TrivialVacuumApp );

TrivialVacuumApp::TrivialVacuumApp( gui::ImGuiWrapper& imGuiWrapper ) :
        App( imGuiWrapper ),
        environment_( std::make_shared<BasicVacuumEnvironment>()),
        vacuumGui( parseTitle<TrivialVacuumApp>(), &stayOpen_, std::string_view()) {
    TRACE;

    vacuumGui.setEnvironment( environment_ );
    environment_->addAgent( agent );
}

void TrivialVacuumApp::renderImpl() {
    TRACE;

    vacuumGui.render( imGuiWrapper());
}

aima::core::Environment& TrivialVacuumApp::environment() noexcept { return *environment_; }

aima::viewer::GraphicViewer& aima::apps::TrivialVacuumApp::viewer() noexcept { return vacuumGui; }
