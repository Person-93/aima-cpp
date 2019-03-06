#pragma once

#include <memory>

#include "viewers/GraphicViewer.hpp"

namespace aima::core {
    class Environment;
}

namespace aima::vacuum {
    class VacuumGui : public viewer::GraphicViewer {
        using viewer::GraphicViewer::GraphicViewer;
    public:
        void setEnvironment( const std::weak_ptr<core::Environment>& environment ) override;

    protected:
        void renderDisplay( std::shared_ptr<core::Environment>& environment ) override;
    };
}


