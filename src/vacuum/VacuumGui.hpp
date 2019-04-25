#pragma once

#include <memory>
#include <string_view>
#include "views/GraphicViewer.hpp" // IWYU pragma: export

namespace aima::core { class Environment; }
namespace aima::gui { struct ImGuiWrapper; }

namespace aima::vacuum {
    class BasicVacuumEnvironment;

    /**
     * A graphic viewer for displaying a vacuum environment
     */
    class VacuumGui : public viewer::GraphicViewer {
    public:
        explicit VacuumGui( std::string_view title, bool* open, std::string_view str_id = std::string_view{} );

        void setEnvironment( const std::shared_ptr<core::Environment>& environment ) override;

    protected:
        void renderDisplay( gui::ImGuiWrapper& imGuiWrapper, std::shared_ptr<core::Environment>& environment ) override;

    private:
        void initMethod( gui::ImGuiWrapper& imGuiWrapper ) override;

        void renderPerformanceMeasure( aima::vacuum::BasicVacuumEnvironment& environment );

        void renderGrid( const BasicVacuumEnvironment& environment );

        std::string littleBuffer;
        std::string bigBuffer;
        ImTextureID dustyTexture;
    };
}
