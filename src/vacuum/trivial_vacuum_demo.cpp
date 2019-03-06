#include <string>

#include "viewers/SimpleViewer.hpp"
#include "trivial_vacuum_demo.hpp"
#include "BasicVacuumEnvironment.hpp" // IWYU pragma: associated
#include "BasicVacuumAgent.hpp"
#include "util/ThreadSafeWrapper.hpp"

using namespace aima::vacuum;
using aima::viewer::SimpleViewer;

#ifndef AIMA_INTEGRATED_APP

#include <iostream>

int main() {
    aima::demos::trivial_vacuum_demo( std::cout );
}

#endif

void aima::demos::trivial_vacuum_demo( std::ostream& out ) {
    BasicVacuumEnvironment environment;
    BasicVacuumAgent       agent;
    SimpleViewer           viewer( out );

    environment.addAgent( agent );
    environment.addEnvironmentView( viewer );

    environment.stepUntilDone();
    environment.notifyViews( "Performance=" + std::to_string( environment.getPerformanceMeasure( agent ).cleanRead()));
}