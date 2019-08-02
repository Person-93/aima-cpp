#pragma once

#include <memory>
#include <string_view>
#include "views/GraphicViewer.hpp" // IWYU pragma: export
#include "BasicVacuumEnvironment.hpp"

namespace aima::core { class Environment; }
namespace aima::gui { struct ImGuiWrapper; }

namespace aima::vacuum {
    class BasicVacuumEnvironment;

    /**
     * A graphic viewer for displaying a vacuum environment
     */
    class VacuumGui final : public viewer::GraphicViewer {
    public:
        explicit VacuumGui( std::string_view title, bool* open, std::string_view str_id = std::string_view{} );

        void setEnvironment( const std::shared_ptr<core::Environment>& environment ) override;

    protected:
        void renderDisplay( gui::ImGuiWrapper& imGuiWrapper, std::shared_ptr<core::Environment>& environment ) override;

    private:
        void initMethod( gui::ImGuiWrapper& imGuiWrapper ) override;

        void renderPerformanceMeasure( aima::vacuum::BasicVacuumEnvironment& environment );

        void renderGrid( const BasicVacuumEnvironment& environment );

        void renderGridCells( float boxSize,
                              float borderWidth,
                              ImDrawList* drawList,
                              const BasicVacuumEnvironment::Locations& locations,
                              const ImVec2& cursor ) const;

        void renderAgentPlan( float boxSize,
                              ImDrawList* drawList,
                              const ImVec2& cursor,
                              const core::Agent& agent,
                              const Location& location ) const;

        void renderAgent( float boxSize,
                          ImDrawList* drawList,
                          const ImVec2& cursor,
                          const BasicVacuumEnvironment::AgentState& state,
                          const Location& location ) const;

        std::string littleBuffer{};
        std::string bigBuffer{};
        ImTextureID dustyTexture{};
        ImTextureID targetTexture{};
    };
}
