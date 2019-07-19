#include "PathFinderApp.hpp"
#include "util/parseTitle.hpp"

using namespace aima::core;
using namespace aima::path_finder;
using aima::apps::PathFinderApp;

PathFinderApp::PathFinderApp( aima::gui::ImGuiWrapper& imGuiWrapper ) :
        App{ imGuiWrapper },
        gui{ util::parseTitle<PathFinderApp>(), &stayOpen_ },
        env{ std::make_shared<PathFinderEnvironment>() },
        agent{} {
    viewer().setEnvironment( env );
    env->addAgent( agent );
}

void aima::apps::PathFinderApp::renderImpl() {
    gui.render( imGuiWrapper());
}
