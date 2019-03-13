#include <stdexcept>
#include <sstream>
#include <iomanip>

#include "VacuumGui.hpp"
#include "BasicVacuumEnvironment.hpp"
#include "core/Environment.hpp"
#include "gui/ImGuiWrapper.hpp"

using namespace aima::core;
using namespace aima::vacuum;

void VacuumGui::setEnvironment( const std::shared_ptr<Environment>& environment ) {
    auto p = std::dynamic_pointer_cast<BasicVacuumEnvironment>( environment );
    if ( !p ) throw std::runtime_error( "VacuumGui expects a vacuum environment" );
    GraphicViewer::setEnvironment( environment );
}

void VacuumGui::renderDisplay( std::shared_ptr<Environment>& environment ) {
    if ( !environment ) {
        ImGui::Text( "The environment has not been set" );
        return;
    }
    auto env = std::dynamic_pointer_cast<BasicVacuumEnvironment>( environment );
    if ( !env ) throw std::runtime_error( "Vacuum Gui got unknown environment type" );

    renderPerformanceMeasure( *env );
    renderGrid( *env );
}

void VacuumGui::renderPerformanceMeasure( BasicVacuumEnvironment& environment ) {
    std::ostringstream ss;
    for ( const auto&[agent, score] : environment.getPerformanceMeasures()) {
        std::ostringstream inner;
        inner << "Agent " << agent.get().id() << " score:";
        ss << '\t'
           << std::left << std::setw( 15 ) << inner.str()
           << std::right << std::setw( 5 ) << score.dirtyRead()
           << '\n';
    }
    ss << '\n';
    ImGui::Text( "%s", ss.str().c_str());
}

void VacuumGui::renderGrid( BasicVacuumEnvironment& environment ) {
    auto& locations = environment.getLocations();

    for ( unsigned i = 0; i < locations.size1(); ++i ) {
        for ( unsigned j = 0; j < locations.size2(); ++j ) {
            bool agentHere =
                         environment.getAgentLocation( environment.getAgents().begin().operator*()) == Location{ i, j };

            std::ostringstream ss;
            ss << locations( i, j );
            if ( agentHere ) ss << '*';
            static const ImVec2 size( 60, 60 );
            ImGui::Button( ss.str().c_str(), size );
        }
        ImGui::SameLine();
    }
}
