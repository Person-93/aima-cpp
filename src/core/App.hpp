#pragma once

#include <atomic>

namespace aima::gui { struct ImGuiWrapper; }
namespace aima::core { class Environment; }
namespace aima::viewer { class GraphicViewer; }

namespace aima::core {
    /**
     * An App that can be run as a standalone or as part of an integrated app
     */
    class App {
    public:
        explicit App( gui::ImGuiWrapper& imGuiWrapper );

        App() = delete;

        App( const App& ) = delete;

        App( App&& ) = delete;

        App& operator=( const App& ) = delete;

        App& operator=( App&& ) = delete;

        virtual ~App() = default;

        /**
         * Renders the app
         */
        virtual void render() = 0;

        [[nodiscard]]  bool display() const noexcept { return display_; }

        void display( bool display ) noexcept;

        [[nodiscard]] bool stayOpen() const noexcept { return stayOpen_; }

        virtual Environment& environment() noexcept = 0;

        virtual viewer::GraphicViewer& viewer() noexcept = 0;

    protected:
        bool stayOpen_ = true;

        gui::ImGuiWrapper& imGuiWrapper() noexcept { return imGuiWrapper_; }

    private:
        std::atomic_bool display_ = true;
        gui::ImGuiWrapper& imGuiWrapper_;
    };
}
