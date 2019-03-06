#pragma once

#include <memory>
#include <string_view>
#include <functional>
#include <string>

#include "imgui.h" // IWYU pragma: export

namespace aima::gui {
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

        struct WindowConfig {
            std::shared_ptr<std::string> title;
            bool* open = nullptr;
            ImGuiWindowFlags flags = 0;
        };

        bool window( WindowConfig& config, std::function<void()> function );

        struct ChildWindowConfig {
            std::shared_ptr<std::string> str_id;
            ImVec2                       size;
            bool                         border;
            ImGuiWindowFlags             flags;
        };

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
    };
}


