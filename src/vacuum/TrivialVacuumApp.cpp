#include <boost/type_index.hpp>

#include "TrivialVacuumApp.hpp"

namespace aima::gui {
    class ImGuiWrapper;
}
using namespace aima::gui;
using namespace aima::vacuum;
using namespace aima::util;

#ifndef AIMA_INTEGRATED_APP

#include "gui/standAloneApp.hpp"

int main() {
    standAloneApp<TrivialVacuumApp>();
    return 0;
}

#endif

TrivialVacuumApp::TrivialVacuumApp( ImGuiWrapper& imGuiWrapper, bool* open ) :
        vacuumGui( imGuiWrapper, parseTitle<TrivialVacuumApp>(), open ) {
    vacuumGui.setEnvironment( environment );
    environment->addAgent( agent );
}

void TrivialVacuumApp::render() {
    vacuumGui.render();
}
