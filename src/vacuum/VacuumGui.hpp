#pragma once

#include "viewers/GraphicViewer.hpp"
#include "vacuum/BasicVacuumEnvironment.hpp"

namespace aima::vacuum {
    class BasicVacuumEnvironment;

    class VacuumGui : public viewer::GraphicViewer {
        using viewer::GraphicViewer::GraphicViewer;
    public:
        void setEnvironment( const std::weak_ptr<Environment>& environment ) override;

    protected:
        void renderDisplay( std::shared_ptr<Environment>& environment ) override;
    };
}


