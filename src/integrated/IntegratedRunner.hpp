#pragma once

#include <vector>
#include <atomic>
#include "core/AppPtr.hpp"
#include "forward.hpp"

namespace aima::gui { struct ImGuiWrapper; }

namespace aima::IntegratedRunner {
    /**
     * Represents an integrated runner that can run apps and demos
     */
    class IntegratedRunner {
    public:
        IntegratedRunner();

        IntegratedRunner( const IntegratedRunner& ) = delete;

        IntegratedRunner( IntegratedRunner&& ) = delete;

        IntegratedRunner& operator=( const IntegratedRunner& ) = delete;

        IntegratedRunner& operator=( IntegratedRunner&& ) = delete;

        /**
         * Show the window with the menu bar for running apps and demos
         * @param imGuiWrapper
         * @param shouldRun
         */
        void run( gui::ImGuiWrapper& imGuiWrapper, std::atomic_bool& shouldRun );

    private:
        bool menuBar( gui::ImGuiWrapper& imGuiWrapper );

        std::vector<AppPtr>            apps;
        std::vector<IntegratedDemoPtr> demos;
    };
}
