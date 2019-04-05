#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include "util/ThreadSafeWrapper.hpp"
#include "gui/OutputConsoleWidget.hpp" // IWYU pragma: keep
#include "forward.hpp"

namespace aima::gui { struct ImGuiWrapper; }

namespace aima::IntegratedRunner {
    namespace detail { struct DemoWindowConfigs; }

    /**
     * Represents a demo that can be run as part of an integrated application
     */
    class IntegratedDemo {
    public:
        /**
         *
         * @param name The name of the demo
         * @param str_id A string that uniquely identifies this demo
         * @param imGuiWrapper Reference to ImGuiWrapper
         * @param factory The function that will make the demo on demand
         */
        IntegratedDemo( std::string_view name,
                        std::string_view str_id,
                        gui::ImGuiWrapper& imGuiWrapper,
                        DemoFactory factory );

        /**
         * @return <code>true</code> if the demo is currently executing,
         *         <code>false</code> otherwise.
         */
        [[nodiscard]] bool isRunning() const;

        /**
         * Renders the demo using ImGui.
         *
         * This method is not thread safe, it should only be called from the owning thread
         * @param imGuiWrapper
         */
        void render();

        /**
         * @return <code>true</code> if the demo "wants" to be rendered,
         *         <code>false</code> otherwise.
         */
        [[nodiscard]] bool display() const;

        /**
         * Sets whether or not the demo "wants" to be rendered
         * @param display
         */
        void display( bool display );

        /**
         * Will return false if the user clicks the 'x' on the demo
         * @return
         */
        [[nodiscard]] bool stayOpen() const noexcept { return stayOpen_; }

        ~IntegratedDemo();

        /**
         * This method is not thread safe, it should only be called from the owning thread
         * @param str_id
         */
        void setStrId( std::string_view str_id );

    protected:
        gui::ImGuiWrapper& imGuiWrapper() noexcept { return imGuiWrapper_; }

    private:
        void runDemo();

        mutable std::mutex mutex;
        const DemoFactory  factory;
        gui::ImGuiWrapper& imGuiWrapper_;
        std::optional<gui::OutputConsoleWidget>    consoleWidget;
        std::unique_ptr<detail::DemoWindowConfigs> windowConfigs;
        std::string                                str_id;
        const std::string_view                     name;
        bool                                       stayOpen_;
    };
}
