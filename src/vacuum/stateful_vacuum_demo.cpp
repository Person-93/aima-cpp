#include <iostream>

#include <viewers/SimpleViewer.hpp>

#include "BasicVacuumEnvironment.hpp"
#include "StatefulVacuumAgent.hpp"

using namespace aima::vacuum;
using namespace aima::viewer;

int main() {
    BasicVacuumEnvironment                                      environment;
    StatefulVacuumAgent<environment.getX(), environment.getY()> agent;
    SimpleViewer                                                viewer( std::cout );

    environment.addAgent( agent );
    environment.addEnvironmentView( viewer );

    environment.stepUntilDone();
    environment.notifyViews( "Performance=" + std::to_string( environment.getPerformanceMeasure( agent )));
}
