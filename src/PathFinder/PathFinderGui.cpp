#include "PathFinderGui.hpp"
#include <utility>
#include "util/define_logger.hpp"
#include "core/Exception.hpp"
#include "PathFinderEnvironment.hpp"
#include "util/parseTitle.hpp"
#include "util/AssetManager.hpp"
#include "util/random_string.hpp"
#include "PathFinderAgent.hpp"
#include "SearchNode.hpp"

using namespace aima::core;
using namespace aima::path_finder;

DEFINE_LOGGER( PathFinderGui )

namespace {
    PathFinderGui::AssetHandles assetLoader() { return {}; }
}

PathFinderGui::PathFinderGui( std::string_view title, bool* open, std::string_view str_id ) :
        GraphicViewer{ assetLoader, title, open, str_id },
        childWindowConfig{
                .str_id = str_id.empty() ? util::random_string( 10 ) : std::string{ str_id },
                .size = { 850, 350 },
                .border = true
        } { TRACE; }

void PathFinderGui::setEnvironment( const std::shared_ptr<Environment>& environment ) {
    TRACE;

    auto p = std::dynamic_pointer_cast<PathFinderEnvironment>( environment );
    if ( !p ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << EnvironmentViewType( util::parseTitle<PathFinderGui>())
                                          << Because( "Received unrecognized environment" ));
    }
    GraphicViewer::setEnvironment( environment );
}

void PathFinderGui::renderDisplay( aima::gui::ImGuiWrapper& imGuiWrapper, std::shared_ptr<Environment>& environment ) {
    if ( !environment ) {
        ImGui::Text( "The environment has not been set" );
        return;
    }

    auto env = std::dynamic_pointer_cast<PathFinderEnvironment>( environment );
    if ( !env ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << EnvironmentViewType( util::parseTitle<PathFinderGui>())
                                          << Because( "Received unrecognized environment" ));
    }

    renderInfo( *env, imGuiWrapper );
    renderPathArea( *env, imGuiWrapper );
}

void PathFinderGui::renderInfo( const PathFinderEnvironment& env, gui::ImGuiWrapper& imGuiWrapper ) const {
    bool isFirst = true;
    for ( const core::Agent& agent: env.getAgents()) {
        const auto& pathFinderAgent = dynamic_cast<const PathFinderAgent&>(agent);
        const auto& status          = pathFinderAgent.getStatus();
        ImGui::TextColored( ImColor{ 0, 100, 255 }, "%s:", util::parseTitle( agent ).data());
        ImGui::Text( "  Nodes in memory:     %zu", status.nodesInMemory );
        ImGui::Text( "  Max nodes in memory: %zu", status.maxNodesInMemory );
        ImGui::Text( "  Nodes generated:     %zu", status.nodesGenerated );
        ImGui::Text( "  Path length:         %.0f", status.pathLength );
        ImGui::Text( "  Time spent thinking: %zu", status.timeSpent );
    }
}

void PathFinderGui::renderPathArea( const PathFinderEnvironment& env, gui::ImGuiWrapper& imGuiWrapper ) {
    imGuiWrapper.childWindow( childWindowConfig, [ & ]() {
        renderObstacles( env );
        renderAgents( env );
        renderGoal( env );
        renderPlans( env );
    } );
}

void PathFinderGui::renderObstacles( const PathFinderEnvironment& env ) const {
    static const ImU32 color    = ImColor{ ImVec4{ 1.0f, 1.0f, 0.4f, 1.0f }};
    const ImVec2       position = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    std::vector<ImVec2> vertices;
    for ( const auto& obstacle : env.getObstacles()) {
        std::transform( obstacle.getPoints().begin(), obstacle.getPoints().end(), std::back_inserter( vertices ),
                        [ &position ]( const Point& point ) {
                            return ImVec2{ point.x + position.x, point.y + position.y };
                        } );
        drawList->AddConvexPolyFilled( vertices.data(), static_cast<const int>(vertices.size()), color );
        vertices.clear();
    }
}

void PathFinderGui::renderAgents( const PathFinderEnvironment& env ) const {
    static const ImU32 color    = ImColor{ ImVec4{ 0.0f, 1.0f, 0.2f, 1.0f }};
    const ImVec2       position = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    for ( const auto&[agent, location] : env.getAgentLocations()) {
        draw_list->AddCircleFilled( ImVec2{ location.x + position.x, location.y + position.y }, 3, color );
    }
}

void PathFinderGui::renderGoal( const PathFinderEnvironment& env ) const {
    static const ImU32 color    = ImColor{ ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }};
    const ImVec2       position = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const auto& goal      = env.getGoal();
    draw_list->AddCircleFilled( ImVec2{ goal.x + position.x, goal.y + position.y }, 3, color );
}

void PathFinderGui::renderPlans( const PathFinderEnvironment& env ) const {
    static const ImU32 color     = ImColor{ ImVec4{ 0.2f, 0.0f, 1.0f, 1.0f }};
    static const float thickness = 2.5;
    const ImVec2       position  = ImGui::GetCursorScreenPos();
    ImDrawList       * drawList = ImGui::GetWindowDrawList();
    for ( const Agent& agent : env.getAgents()) {
        auto p = dynamic_cast<const PathFinderAgent*>(&agent);
        if ( !p ) {
            using namespace aima::core::exception;
            AIMA_THROW_EXCEPTION( Exception{} << EnvironmentViewType( util::parseTitle<PathFinderGui>())
                                              << Because( "Received unrecognized agent" ));
        }

        auto previous = p->getPlan();
        if ( !previous ) continue;

        for ( auto current = previous->parent.lock();
              current;
              previous = current, current = current->parent.lock()) {
            drawList->AddLine( ImVec2{ previous->location.x + position.x, previous->location.y + position.y },
                               ImVec2{ current->location.x + position.x, current->location.y + position.y },
                               color,
                               thickness );
        }
    }
}
