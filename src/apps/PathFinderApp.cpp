#include "PathFinderApp.hpp"
#include "util/parseTitle.hpp"

using namespace aima::core;
using namespace aima::path_finder;
using aima::apps::PathFinderApp;

PathFinderApp::PathFinderApp( aima::gui::ImGuiWrapper& imGuiWrapper ) :
        App{ imGuiWrapper },
        gui{ util::parseTitle<PathFinderApp>(), &stayOpen_ },
        env{ std::make_shared<PathFinderEnvironment>() },
        agentPicker{ *env, &stayOpen_ } {
    viewer().setEnvironment( env );
}

void aima::apps::PathFinderApp::renderImpl() {
    agentPicker.done() ? gui.render( imGuiWrapper()) : agentPicker.render( imGuiWrapper());
}
