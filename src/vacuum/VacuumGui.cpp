#include <stdexcept>

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

void renderPerformanceMeasure( const BasicVacuumEnvironment::PerformanceMeasures& environment );

void renderGrid( const BasicVacuumEnvironment::Locations& locations );

void VacuumGui::renderDisplay( std::shared_ptr<Environment>& environment ) {
    auto env = std::dynamic_pointer_cast<BasicVacuumEnvironment>( environment );
    if ( !env ) {
        ImGui::Text( "The environment has not been set" );
        return;
    }

    renderPerformanceMeasure( env->getPerformanceMeasures());
    renderGrid( env->getLocations());
}

void renderPerformanceMeasure( const BasicVacuumEnvironment::PerformanceMeasures& environment ) {
    ImGui::Text( "This is where the scores will be rendered" );
}

void renderGrid( const BasicVacuumEnvironment::Locations& locations ) {
    ImGui::Text( "This is where the grid will be rendered" );
}
