#pragma once

#include <memory>

#include "viewers/GraphicViewer.hpp"

namespace aima::core {
    class Environment;
}

namespace aima::vacuum {
    class BasicVacuumEnvironment;
}

namespace aima::vacuum {
    class VacuumGui : public viewer::GraphicViewer {
        using viewer::GraphicViewer::GraphicViewer;
    public:
        void setEnvironment( const std::shared_ptr<core::Environment>& environment ) override;

    protected:
        void renderDisplay( std::shared_ptr<core::Environment>& environment ) override;

    private:
        void renderPerformanceMeasure( aima::vacuum::BasicVacuumEnvironment& environment );

        void renderGrid( aima::vacuum::BasicVacuumEnvironment& environment );
    };
}


