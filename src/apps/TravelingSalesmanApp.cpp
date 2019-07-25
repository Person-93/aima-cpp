#include "TravelingSalesmanApp.hpp"
#include "util/parseTitle.hpp"
#include "TravelingSalesman/TravelingSalesmanEnvironment.hpp"

using aima::apps::TravelingSalesmanApp;
using namespace aima::traveling_salesman;

TravelingSalesmanApp::TravelingSalesmanApp( gui::ImGuiWrapper& imGuiWrapper ) :
        App{ imGuiWrapper },
        viewer_{ util::parseTitle<TravelingSalesmanApp>(), &stayOpen_ },
        env{ new TravelingSalesmanEnvironment } {
    viewer_.setEnvironment( env );
    env->addAgent( agent );
}

void TravelingSalesmanApp::renderImpl() {
    viewer_.render( imGuiWrapper());
}
