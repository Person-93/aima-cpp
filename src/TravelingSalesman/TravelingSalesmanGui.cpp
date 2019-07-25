#include "TravelingSalesmanGui.hpp"
#include <sstream>
#include "util/AssetManager.hpp"
#include "util/random_string.hpp"
#include "util/define_logger.hpp"
#include "core/Exception.hpp"
#include "IterativeLengtheningAgent.hpp"
#include "util/parseTitle.hpp"

using namespace aima::traveling_salesman;

DEFINE_LOGGER( TravelingSalesmanGui )

TravelingSalesmanGui::TravelingSalesmanGui( std::string_view title, bool* open, std::string_view str_id ) :
        GraphicViewer( [] { return AssetHandles{}; }, title, open, str_id ),
        childWindowConfig{
                .str_id = util::random_string( 5 ),
                .size   = { 800, 600 },
                .border = true,
                .flags  = ImGuiWindowFlags_NoResize,
        } {}

void TravelingSalesmanGui::renderDisplay( aima::gui::ImGuiWrapper& imGuiWrapper,
                                          std::shared_ptr<aima::core::Environment>& environment ) {
    if ( !environment ) {
        ImGui::TextUnformatted( "The environment has not been set" );
        return;
    }

    auto env = std::dynamic_pointer_cast<TravelingSalesmanEnvironment>( environment );
    if ( !env ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << Because{ "Received environment of unrecognized type" } );
    }

    renderInfo( imGuiWrapper, *env );
    imGuiWrapper.childWindow( childWindowConfig, [ & ] {
        renderLocations( imGuiWrapper, *env );
        renderPlan( imGuiWrapper, *env );
    } );
}

void TravelingSalesmanGui::renderInfo( gui::ImGuiWrapper& imGuiWrapper, const TravelingSalesmanEnvironment& env ) {
    for ( const core::Agent& agent: env.getAgents()) {
        const auto& travelingSalesAgent = dynamic_cast<const IterativeLengtheningAgent&>(agent);
        const auto& status              = travelingSalesAgent.getStatus();
        static const ImColor color{ 0, 100, 255 };

        std::string visitedText;
        if ( const auto& plan = travelingSalesAgent.getPlan()) {
            std::ostringstream ss;
            for ( size_t       i : plan->visited ) {
                ss << env.getLocations()[ i ] << ", ";
            }

            visitedText = ss.str();
            visitedText.erase( visitedText.end() - 1, visitedText.end());
        }

        ImGui::TextColored( color, "%s:", util::parseTitle( agent ).data());
        ImGui::Text( "  Nodes in memory:     %zu", status.nodesInMemory );
        ImGui::Text( "  Max nodes in memory: %zu", status.maxNodesInMemory );
        ImGui::Text( "  Nodes generated:     %zu", status.nodesGenerated );
        ImGui::Text( "  Path length:         %.0f", status.pathLength );
        ImGui::Text( "  Time spent thinking: %zu", status.timeSpent );
        ImGui::Text( "  Locations visited:   %s", visitedText.data());
    }
}

void TravelingSalesmanGui::renderLocations( gui::ImGuiWrapper& imGuiWrapper, const TravelingSalesmanEnvironment& env ) {
    static const ImColor color{ 100, 225, 0 };
    static const ImColor startColor{ 0, 100, 225 };
    auto                 cursor   = ImGui::GetCursorScreenPos();
    auto                 drawList = ImGui::GetWindowDrawList();
    const auto      & locations = env.getLocations();
    for ( const auto& location: locations ) {
        drawList->AddCircleFilled( { location.x + cursor.x + 50, location.y + cursor.y + 50 }, 3, color );
    }
    const auto      & start     = locations.front();
    drawList->AddCircleFilled( { start.x + cursor.x + 50, start.y + cursor.y + 50 }, 3,
                               startColor );
}

void TravelingSalesmanGui::renderPlan( gui::ImGuiWrapper& imGuiWrapper, const TravelingSalesmanEnvironment& env ) {
    static const ImColor color{ 100, 0, 225 };
    auto                 cursor   = ImGui::GetCursorScreenPos();
    auto                 drawList = ImGui::GetWindowDrawList();
    for ( const core::Agent& agent : env.getAgents()) {
        const auto plan = dynamic_cast<const IterativeLengtheningAgent&>(agent).getPlan();
        if ( !plan ) continue;

        const auto& visited = plan->visited;
        for ( size_t i = 0; i < visited.size() - 1; ++i ) {
            const auto& locationA = env.getLocations()[ visited[ i ]];
            const auto& locationB = env.getLocations()[ visited[ i + 1 ]];
            drawList->AddLine( { locationA.x + cursor.x + 50, locationA.y + cursor.y + 50 },
                               { locationB.x + cursor.x + 50, locationB.y + cursor.y + 50 },
                               color );
        }
    }
}
