#include "StatefulVacuumApp.hpp"
#include "util/parseTitle.hpp"
#include "../vacuum/BasicVacuumEnvironment.hpp"
#include "util/define_logger.hpp"


using namespace aima::vacuum;
using namespace aima::gui;
using aima::apps::StatefulVacuumApp;

DEFINE_LOGGER( StatefulVacuumApp );

StatefulVacuumApp::StatefulVacuumApp( ImGuiWrapper& imGuiWrapper )
        : App( imGuiWrapper ),
          environment_( std::make_shared<BasicVacuumEnvironment>()),
          agent( *environment_ ),
          vacuumGui( util::parseTitle<StatefulVacuumApp>(), &stayOpen_ ) {
    TRACE;

    vacuumGui.setEnvironment( environment_ );
    environment_->addAgent( agent );
}

void StatefulVacuumApp::render() {
    TRACE;

    vacuumGui.render( imGuiWrapper());
}

aima::core::Environment& StatefulVacuumApp::environment() noexcept { return *environment_; }

aima::viewer::GraphicViewer& aima::apps::StatefulVacuumApp::viewer() noexcept { return vacuumGui; }
