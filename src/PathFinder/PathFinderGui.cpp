#include "PathFinderGui.hpp"
#include <utility>
#include "util/define_logger.hpp"
#include "core/Exception.hpp"
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

    const ImColor& color() {
        static std::array<ImColor, 3> colors{
                ImColor{ ImVec4{ 0, 100, 255, .5 }},
                ImColor{ ImVec4{ 255, 0, 100, .5 }},
                ImColor{ ImVec4{ 100, 255, 0, .5 }},
        };
        static size_t                 colorsIndex{};
        if ( colorsIndex >= colors.size()) colorsIndex = 0;
        return colors[ colorsIndex++ ];
    }
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

void PathFinderGui::agentAdded( const aima::core::Agent& agent, const aima::core::Environment& source ) {
    agentColors.insert( { agent, color() } );
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
    for ( const core::Agent& agent: env.getAgents()) {
        const auto& pathFinderAgent = dynamic_cast<const PathFinderAgent&>(agent);
        const auto& status          = pathFinderAgent.getStatus();
        ImGui::TextColored( agentColors.at( agent ), "%s:", util::parseTitle( agent ).data());
        ImGui::Text( "  Nodes in memory:     %zu", status.nodesInMemory );
        ImGui::Text( "  Max nodes in memory: %zu", status.maxNodesInMemory );
        ImGui::Text( "  Nodes generated:     %zu", status.nodesGenerated );
        ImGui::Text( "  Path length:         %.0f", status.pathLength );
        ImGui::Text( "  Time spent thinking: %zu", status.timeSpent );
    }
}

void PathFinderGui::renderPathArea( const PathFinderEnvironment& env, gui::ImGuiWrapper& imGuiWrapper ) {
    auto  region       = ImGui::GetContentRegionAvail();
    float xScaleFactor = region.x / 800;
    float yScaleFactor = region.y / 350;
    float scaleFactor  = std::min( xScaleFactor, yScaleFactor );
    childWindowConfig.size = { 800 * scaleFactor, 350 * scaleFactor };
    imGuiWrapper.childWindow( childWindowConfig, [ & ]() {
        renderObstacles( env, scaleFactor );
        renderAgents( env, scaleFactor );
        renderGoal( env, scaleFactor );
        renderPlans( env, scaleFactor );
    } );
}

void PathFinderGui::renderObstacles( const PathFinderEnvironment& env, float scaleFactor ) const {
    static const ImU32 color    = ImColor{ ImVec4{ 0.4f, 0.4f, 0.4f, 1.0f }};
    const ImVec2       position = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    std::vector<ImVec2> vertices;
    for ( const auto& obstacle : env.getObstacles()) {
        std::transform( obstacle.getPoints().begin(), obstacle.getPoints().end(), std::back_inserter( vertices ),
                        [ & ]( const util::geometry::Point& point ) {
                            return ImVec2{ point.x * scaleFactor + position.x,
                                           point.y * scaleFactor + position.y };
                        } );
        drawList->AddConvexPolyFilled( vertices.data(), static_cast<const int>(vertices.size()), color );
        vertices.clear();
    }
}

void PathFinderGui::renderAgents( const PathFinderEnvironment& env, float scaleFactor ) const {
    static const ImU32 color    = ImColor{ ImVec4{ 0.0f, 1.0f, 0.2f, 1.0f }};
    const ImVec2       position = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    for ( const auto&[agent, location] : env.getAgentLocations()) {
        draw_list->AddCircleFilled( ImVec2{ location.x * scaleFactor + position.x,
                                            location.y * scaleFactor + position.y },
                                    5 * scaleFactor,
                                    color );
    }
}

void PathFinderGui::renderGoal( const PathFinderEnvironment& env, float scaleFactor ) const {
    static const ImU32 color    = ImColor{ ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }};
    const ImVec2       position = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const auto& goal      = env.getGoal();
    draw_list->AddCircleFilled( ImVec2{ goal.x * scaleFactor + position.x,
                                        goal.y * scaleFactor + position.y },
                                5 * scaleFactor,
                                color );
}

void PathFinderGui::renderPlans( const PathFinderEnvironment& env, float scaleFactor ) const {
    static float thickness = 2.5 * scaleFactor;
    const ImVec2 position  = ImGui::GetCursorScreenPos();
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

        const auto& color = agentColors.at( agent );
        for ( auto current = previous->parent.lock();
              current;
              previous = current, current = current->parent.lock()) {
            drawList->AddLine( ImVec2{ previous->location.x * scaleFactor + position.x,
                                       previous->location.y * scaleFactor + position.y },
                               ImVec2{ current->location.x * scaleFactor + position.x,
                                       current->location.y * scaleFactor + position.y },
                               color,
                               thickness );
        }
    }
}
