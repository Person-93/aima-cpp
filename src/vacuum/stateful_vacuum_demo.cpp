#include "viewers/SimpleViewer.hpp"
#include "BasicVacuumEnvironment.hpp"
#include "StatefulVacuumAgent.hpp"
#include "stateful_vacuum_demo.hpp"

using namespace aima::vacuum;
using namespace aima::viewer;

#ifndef AIMA_INTEGRATED_APP

#include <iostream>

int main() {
    aima::demos::stateful_vacuum_demo( std::cout );
}

#endif

void aima::demos::stateful_vacuum_demo( std::ostream& out ) {
    BasicVacuumEnvironment environment;
    StatefulVacuumAgent    agent( environment.getX(), environment.getY());
    SimpleViewer           viewer( std::cout );

    environment.addAgent( agent );
    environment.addEnvironmentView( viewer );

    environment.stepUntilDone();
    environment.notifyViews( "Performance=" + std::to_string( environment.getPerformanceMeasure( agent ).cleanRead()));
}
