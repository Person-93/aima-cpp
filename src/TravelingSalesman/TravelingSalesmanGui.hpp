#pragma once

#include "views/GraphicViewer.hpp"
#include "TravelingSalesmanEnvironment.hpp"

namespace aima::traveling_salesman {
    class TravelingSalesmanGui final : public viewer::GraphicViewer {
    public:
        explicit TravelingSalesmanGui( std::string_view title,
                                       bool* open = nullptr,
                                       std::string_view str_id = std::string_view{} );

    private:
        void renderDisplay( gui::ImGuiWrapper& imGuiWrapper, std::shared_ptr<core::Environment>& environment ) override;

        static void renderInfo( gui::ImGuiWrapper& imGuiWrapper, const TravelingSalesmanEnvironment& env );

        static void renderLocations( gui::ImGuiWrapper& imGuiWrapper,
                                     const TravelingSalesmanEnvironment& env,
                                     float scale );

        static void renderPlan( gui::ImGuiWrapper& imGuiWrapper,
                                const TravelingSalesmanEnvironment& env,
                                float scale );

        gui::ChildWindowConfig childWindowConfig;
    };
}
