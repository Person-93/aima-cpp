#pragma once

#include <string_view>
#include <functional>
#include <string>
#include "imgui.h" // IWYU pragma: export

namespace aima::gui {
    struct WindowConfig {
        std::string title;
        bool* open = nullptr;
        ImGuiWindowFlags flags = 0;
    };

    struct ChildWindowConfig {
        std::string      str_id;
        ImVec2           size;
        bool             border;
        ImGuiWindowFlags flags;
    };

    /**
     * An RAII class to initialize and shutdown ImGui
     */
    struct ImGuiWrapper {
        explicit ImGuiWrapper( std::string_view title = "" );

        ~ImGuiWrapper();

        bool shouldClose();

        void setWindowTitle( std::string_view title );

        /**
         * An RAII class to start and end a frame in ImGui
         */
        class Frame {
            Frame();

            friend ImGuiWrapper;
        public:
            ~Frame();
        };

        Frame frame();

        bool window( WindowConfig& config, std::function<void()> function );

        bool childWindow( ChildWindowConfig& config, std::function<void()> function );

        class DisableControls {
        private:
            friend ImGuiWrapper;

            explicit DisableControls( bool disable );

        public:
            void disable();

            void enable();

            ~DisableControls();

        private:
            bool disabled;
        };

        DisableControls disableControls( bool disable = true );

        bool mainMenu( std::function<void()> function );

        void menu( std::string_view label, bool enabled, std::function<void()> function );

        void menuItem( std::string_view label, bool selected, bool enabled, std::function<void()> function );
    };


}
