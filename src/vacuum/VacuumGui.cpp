#include "VacuumGui.hpp"
#include <iomanip>
#include <functional>
#include <string>
#include <boost/numeric/ublas/matrix.hpp>
#include "vacuum/BasicVacuumEnvironment.hpp"
#include "vacuum/SearchBasedAgent.hpp"
#include "core/Agent.hpp"
#include "core/Exception.hpp"
#include "gui/ImGuiWrapper.hpp"
#include "misc.hpp"
#include "util/parseTitle.hpp"
#include "util/define_logger.hpp"
#include "util/StringBuilder.hpp"
#include "util/AssetManager.hpp"
#include "gui/Image.hpp"

// IWYU pragma: no_include <type_traits>
// IWYU pragma: no_include <unordered_map>

using namespace aima::core;
using namespace aima::vacuum;
using std::string_view;

DEFINE_LOGGER( VacuumGui )


namespace {
    std::string string( std::string::size_type size ) {
        std::string s;
        s.reserve( size );
        return s;
    }

    namespace Images {
        constexpr std::string_view DUSTY_BACKGROUND = "DUSTY_BACKGROUND";
        constexpr std::string_view TARGET           = "TARGET";
    }

    using ImageData = std::map<std::string_view, const std::filesystem::path>;

    ImageData& imageData() {
        static ImageData data{
                { Images::DUSTY_BACKGROUND, "dusty.jpg" },
                { Images::TARGET,           "target.png" },
        };
        return data;
    }

    VacuumGui::AssetHandles loadAssets() {
        VacuumGui::AssetHandles assetHandles;
        assetHandles.reserve( imageData().size());

        std::transform( imageData().begin(), imageData().end(), std::back_inserter( assetHandles ),
                        []( const auto& pair ) {
                            auto[tag, path] = pair;
                            return aima::util::AssetManager::add<aima::gui::Image>( path, tag );
                        } );

        return assetHandles;
    }
}

VacuumGui::VacuumGui( std::string_view title, bool* open, std::string_view str_id ) :
        GraphicViewer( loadAssets, title, open, str_id ),
        littleBuffer( string( 16 )),
        bigBuffer( string( 256 )) {
    TRACE;
    namespace AssetManager = util::AssetManager;
}

void VacuumGui::setEnvironment( const std::shared_ptr<Environment>& environment ) {
    TRACE;

    if ( auto p = std::dynamic_pointer_cast<BasicVacuumEnvironment>( environment ); !p ) {
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

namespace {
    ImVec2 calculateCenterOfLocation( const Location& location, const ImVec2& cursor, float boxSize ) {
        return {
                cursor.x + location.x * boxSize + boxSize / 2,
                cursor.y + location.y * boxSize + boxSize / 2,
        };
    }
}

void VacuumGui::renderGrid( const BasicVacuumEnvironment& environment ) {
    TRACE;

    ImVec2 regionAvail = ImGui::GetContentRegionAvail();
    float  gridSize    = std::min( regionAvail.x, regionAvail.y );
    float  boxSpace    = ( gridSize / std::max( environment.getX(), environment.getY()));
    float  boxSize     = boxSpace * .95f;
    float  borderWidth = boxSpace * .05f;
    ImDrawList* drawList = ImGui::GetWindowDrawList();


    const auto& locations = environment.getLocations();

    auto cursor = ImGui::GetCursorScreenPos();
    renderGridCells( boxSize, borderWidth, drawList, locations, cursor );

    for ( const auto&[agent, state]: environment.getAgentStates()) {
        const auto& location = state.location;
        renderAgentPlan( boxSize, drawList, cursor, agent, location );
        renderAgent( boxSize, drawList, cursor, state, location );

    }
}

void VacuumGui::renderAgent( float boxSize,
                             ImDrawList* drawList,
                             const ImVec2& cursor,
                             const BasicVacuumEnvironment::AgentState& state,
                             const Location& location ) const {// render agents as circles in the center of their location
    ImVec2               circleOrigin = calculateCenterOfLocation( location, cursor, boxSize );
    static const ImColor agentColor( 255, 0, 0 );
    drawList->AddCircleFilled( circleOrigin, boxSize / 3, agentColor, 48 );
    if ( state.sucking ) {
        ImVec2 cornerA( circleOrigin.x - boxSize / 6, circleOrigin.y - boxSize / 6 );
        ImVec2 cornerB( circleOrigin.x + boxSize / 6, circleOrigin.y + boxSize / 6 );
        drawList->AddImage( dustyTexture, cornerA, cornerB );
    }
}

void VacuumGui::renderAgentPlan( float boxSize,
                                 ImDrawList* drawList,
                                 const ImVec2& cursor,
                                 const Agent& agent,
                                 const Location& location ) const {
    if ( !agent.isAlive()) return;
    auto p = dynamic_cast<const SearchBasedAgent*>(&agent);
    if ( !p ) return;

    // if agent is still in planning stage, render plan, otherwise render next stop
    if ( auto destination = p->nextStop(); !destination ) {
        // render little circles at the way-points with lines connecting them
        static const ImColor  planColor{ 0, 225, 100 };
        std::optional<ImVec2> previousCenter;
        for ( auto            plan = p->getPlan(); plan; plan = plan->parent ) {
            ImVec2 locationCenter = calculateCenterOfLocation( plan->location, cursor, boxSize );
            drawList->AddCircleFilled( locationCenter, boxSize / 6, planColor );
            if ( previousCenter )
                drawList->AddLine( locationCenter, *previousCenter, planColor );
            previousCenter = locationCenter;
        }
    }
    else if ( *destination != location ) {
        ImVec2 cornerA{ cursor.x + destination->x * boxSize, cursor.y + location.y * boxSize };
        ImVec2 cornerB{ cornerA.x + boxSize, cornerA.y + boxSize };
        drawList->AddImage( targetTexture, cornerA, cornerB );
    }
}

void VacuumGui::renderGridCells( float boxSize,
                                 float borderWidth,
                                 ImDrawList* drawList,
                                 const BasicVacuumEnvironment::Locations& locations,
                                 const ImVec2& cursor ) const {
    ImU32      bgColor = ImColor(( ImGui::GetStyle().Colors[ ImGuiCol_ChildBg ] ));
    ImU32      col     = ~bgColor;
    for ( auto i       = locations.cbegin1(), end1 = locations.cend1(); i != end1; ++i ) {
        for ( auto j = i.cbegin(), end2 = i.cend(); j != end2; ++j ) {
            ImVec2 cornerA( cursor.x + j.index1() * boxSize, cursor.y + j.index2() * boxSize );
            ImVec2 cornerB( cornerA.x + boxSize, cornerA.y + boxSize );
            if ( *j == LocationState::DIRTY ) drawList->AddImage( dustyTexture, cornerA, cornerB );
            drawList->AddRect( cornerA, cornerB, col, 0, ImDrawCornerFlags_All, borderWidth );
        }
        ImGui::SameLine();
    }
}

void VacuumGui::initMethod( aima::gui::ImGuiWrapper& imGuiWrapper ) {
    using aima::util::AssetManager::get;

    auto& dustyImage = get<gui::Image>( Images::DUSTY_BACKGROUND );
    dustyImage.makeTexture( imGuiWrapper );
    dustyTexture = reinterpret_cast<ImTextureID> (dustyImage.texture());

    auto& targetImage = get<gui::Image>( Images::TARGET );
    targetImage.makeTexture( imGuiWrapper );
    targetTexture = reinterpret_cast<ImTextureID>(targetImage.texture());
}
