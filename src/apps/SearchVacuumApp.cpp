#include "SearchVacuumApp.hpp"
#include "util/parseTitle.hpp"
#include "vacuum/VisibleVacuumEnvironment.cpp"

using aima::apps::SearchVacuumApp;
using namespace aima::vacuum;

constexpr unsigned extent = 3;

SearchVacuumApp::SearchVacuumApp( gui::ImGuiWrapper& imGuiWrapper ) :
        App( imGuiWrapper ),
        viewer_{ util::parseTitle<SearchVacuumApp>(), &stayOpen_ },
        env{ new VisibleVacuumEnvironment{ 3, 3 }},
        picker{ *env, &stayOpen_, [ this ]( const SearchBasedAgent& agent ) {
            auto p = static_cast<VisibleVacuumEnvironment*>(env.get()); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
            p->setAgentLocation( agent, { 1, 1 } );
        }, false } {
    auto           p = std::static_pointer_cast<BasicVacuumEnvironment>( env );
    for ( unsigned i = 0; i < extent; ++i ) {
        p->setLocationState( { i, 0 }, LocationState::DIRTY );
    }
    for ( unsigned i = 1; i < extent; ++i ) {
        for ( unsigned j = 0; j < extent; ++j ) {
            p->setLocationState( { j, i }, LocationState::CLEAN );
        }
    }

    viewer_.setEnvironment( env );
}

void SearchVacuumApp::renderImpl() {
    picker.done() ? viewer_.render( imGuiWrapper()) : picker.render( imGuiWrapper());
}
