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
          agent( static_cast<BasicVacuumEnvironment&> (*environment_)), // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
          vacuumGui( util::parseTitle<StatefulVacuumApp>(), &stayOpen_, std::string_view()) {
    TRACE;

    vacuumGui.setEnvironment( environment_ );
    environment_->addAgent( agent );
}

void StatefulVacuumApp::renderImpl() {
    TRACE;

    vacuumGui.render( imGuiWrapper());
}

std::shared_ptr<aima::core::Environment>& StatefulVacuumApp::environment() noexcept { return environment_; }

aima::viewer::GraphicViewer& aima::apps::StatefulVacuumApp::viewer() noexcept { return vacuumGui; }
