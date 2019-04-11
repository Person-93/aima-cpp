#include "VacuumGui.hpp"
#include <iomanip>
#include <functional>
#include <string>
#include <boost/numeric/ublas/matrix.hpp>
#include "vacuum/BasicVacuumEnvironment.hpp"
#include "core/Agent.hpp"
#include "core/Exception.hpp"
#include "gui/ImGuiWrapper.hpp"
#include "misc.hpp"
#include "util/parseTitle.hpp"
#include "util/define_logger.hpp"
#include "util/StringBuilder.hpp"

// IWYU pragma: no_include <type_traits>
// IWYU pragma: no_include <unordered_map>

using namespace aima::core;
using namespace aima::vacuum;
using std::string_view;

DEFINE_LOGGER( VacuumGui );


namespace {
    std::string string( std::string::size_type size ) {
        std::string s;
        s.reserve( size );
        return s;
    }
}

VacuumGui::VacuumGui( string_view title, bool* open, string_view str_id ) :
        GraphicViewer( title, open, str_id ), littleBuffer( string( 16 )), bigBuffer( string( 256 )) { TRACE; }

void VacuumGui::setEnvironment( const std::shared_ptr<Environment>& environment ) {
    TRACE;
    auto p = std::dynamic_pointer_cast<BasicVacuumEnvironment>( environment );
    if ( !p ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << EnvironmentViewType( util::parseTitle<VacuumGui>())
                                          << Because( "Received unrecognized environment" ));
    }
    GraphicViewer::setEnvironment( environment );
}

void VacuumGui::renderDisplay( gui::ImGuiWrapper& imGuiWrapper, std::shared_ptr<core::Environment>& environment ) {
    TRACE;

    if ( !environment ) {
        ImGui::Text( "The environment has not been set" );
        return;
    }

    auto env = std::dynamic_pointer_cast<BasicVacuumEnvironment>( environment );
    if ( !env ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << EnvironmentViewType( util::parseTitle<VacuumGui>())
                                          << Because( "Received unrecognized environment" ));
    }

    renderPerformanceMeasure( *env );
    renderGrid( *env );
}

void VacuumGui::renderPerformanceMeasure( BasicVacuumEnvironment& environment ) {
    TRACE;

    util::StringBuilder stringBuilder( bigBuffer );
    for ( const auto&[agent, score] : environment.getPerformanceMeasures()) {
        stringBuilder << '\t'
                      << std::left << std::setw( 16 )
                      << ( util::StringBuilder( littleBuffer ) << "Agent " << agent.get().id() << " score:" )
                      << std::right << std::setw( 5 ) << score.dirtyRead()
                      << '\n';
    }
    stringBuilder << '\n';

    ImGui::Text( "%s", std::move( stringBuilder ).toString().c_str());
}

void VacuumGui::renderGrid( const BasicVacuumEnvironment& environment ) {
    TRACE;

    const auto& locations = environment.getLocations();
    const auto agentLocations = environment.getAgentLocationsList();

    for ( auto i = locations.cbegin1(), end1 = locations.cend1(); i != end1; ++i ) {
        for ( auto j = i.cbegin(), end2 = i.cend(); j != end2; ++j ) {
            const bool agentHere = std::binary_search( agentLocations.cbegin(), agentLocations.cend(),
                                                       Location{ j.index1(), j.index2() }, std::less<Location>{} );

            static const ImVec2 size( 60, 60 );
            ImGui::Button(( util::StringBuilder( littleBuffer ) << *j << ( agentHere ? '*' : '\0' ))
                                  .toString().c_str(),
                          size );
        }
        ImGui::SameLine();
    }
}
