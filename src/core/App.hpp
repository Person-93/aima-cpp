#pragma once

#include <atomic>
#include <memory>
#include "util/UniqueIdMixin.hpp"

namespace aima::gui { struct ImGuiWrapper; }
namespace aima::core { class Environment; }
namespace aima::viewer { class GraphicViewer; }

namespace aima::core {
    /**
     * An App that can be run as a standalone or as part of an integrated app
     */
    class App : public util::UniqueIdMixin<App, true> {
    public:
        explicit App( gui::ImGuiWrapper& imGuiWrapper );

        App() = delete;

        App( const App& ) = delete;

        App( App&& ) = delete;

        App& operator=( const App& ) = delete;

        App& operator=( App&& ) = delete;

        virtual ~App();

        void render() {
            if ( display()) renderImpl();
        }

        [[nodiscard]]  bool display() const noexcept { return display_; }

        void display( bool display ) noexcept;

        [[nodiscard]] bool stayOpen() const noexcept { return stayOpen_; }

        virtual std::shared_ptr<Environment>& environment() noexcept = 0;

        virtual viewer::GraphicViewer& viewer() noexcept = 0;

    protected:
        bool stayOpen_ = true;

        gui::ImGuiWrapper& imGuiWrapper() noexcept { return imGuiWrapper_; }

    private:
        /**
         * Renders the app
         */
        virtual void renderImpl() = 0;

        std::atomic_bool display_ = false;
        gui::ImGuiWrapper& imGuiWrapper_;
    };
}
