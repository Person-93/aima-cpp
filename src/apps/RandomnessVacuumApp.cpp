#include "RandomnessVacuumApp.hpp"
#include "util/parseTitle.hpp"
#include "vacuum/BasicVacuumEnvironment.hpp"

using namespace aima::vacuum;

constexpr unsigned long extent = 3;

aima::apps::RandomnessVacuumApp::RandomnessVacuumApp( aima::gui::ImGuiWrapper& imGuiWrapper ) :
        App( imGuiWrapper ),
        viewer_{ util::parseTitle<RandomnessVacuumApp>(), &stayOpen_ },
        env{ new BasicVacuumEnvironment{ extent, extent }},
        agent{ extent, extent } {
    auto           p = std::static_pointer_cast<BasicVacuumEnvironment>( env );
    for ( unsigned i = 0; i < extent; ++i ) {
        p->setLocationState( { i, 0 }, LocationState::DIRTY );
    }
    for ( unsigned i = 1; i < extent; ++i ) {
        for ( unsigned j = 0; j < extent; ++j ) {
            p->setLocationState( { j, i }, LocationState::CLEAN );
        }
    }

    viewer_.setEnvironment( p );
    p->addAgent( agent );
    p->setAgentLocation( agent, { 1, 1 } );
}

void aima::apps::RandomnessVacuumApp::renderImpl() {
    viewer_.render( imGuiWrapper());
}
