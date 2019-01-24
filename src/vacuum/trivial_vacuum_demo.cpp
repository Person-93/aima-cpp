#include <iostream>

#include <viewers/SimpleViewer.hpp>

#include "BasicVacuumEnvironment.hpp"
#include "BasicVacuumAgent.hpp"

using std::cout;
using namespace aima::vacuum;
using aima::viewer::SimpleViewer;

int main() {
    BasicVacuumEnvironment environment;
    BasicVacuumAgent  agent;
    SimpleViewer      viewer( cout );

    environment.addAgent( agent );
    environment.addEnvironmentView( viewer );

    environment.stepUntilDone();
    environment.notifyViews( "Performance=" + std::to_string( environment.getPerformanceMeasure( agent )));
}
