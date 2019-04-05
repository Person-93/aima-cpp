#pragma once

#include <memory>
#include <string_view>
#include <exception>
#include <mutex>
#include <thread>
#include "core/EnvironmentView.hpp"
#include "gui/ImGuiWrapper.hpp"
#include "gui/OutputConsoleWidget.hpp"
#include "util/ThreadSafeWrapper.hpp"
#include "SimpleViewer.hpp"


namespace aima::core {
    class Agent;

    class Percept;

    class Action;

    class Environment;
}

namespace aima::viewer {
    namespace detail {
        struct WindowConfigs {
            gui::WindowConfig      windowConfig;
            gui::ChildWindowConfig consoleAreaConfig;
            gui::ChildWindowConfig scrollingSectionConfig;
        };
    }

    /**
     * A viewer that displays a graphic representation of the environment to the user in a DearImGui window
     */
    class GraphicViewer : public core::EnvironmentView {
    public:
        /**
         * Constructs the viewer
         * @param title The title of the DearImGui window
         * @param open Will be set to false if the user closes the window
         */
        explicit GraphicViewer( std::string_view title,
                                bool* open = nullptr,
                                std::string_view str_id = std::string_view{} );

        void notify( std::string_view message ) override;

        void agentAdded( const core::Agent& agent, const core::Environment& source ) override;

        void agentActed( const core::Agent& agent,
                         const core::Percept& percept,
                         const core::Action& action,
                         const core::Environment& environment ) override;

        virtual void setEnvironment( const std::shared_ptr<core::Environment>& environment );

        /**
         * This method is not thread safe and should only be called from the owning thread
         * @param imGuiWrapper
         * @return
         */
        bool render( gui::ImGuiWrapper& imGuiWrapper );

        /**
         * This method is not thread safe and should only be called from the owning thread
         * @param str_id
         */
        void setStrId( std::string_view str_id );

    protected:
        /**
         * This method is protected to prevent slicing of derived classes
         */
        ~GraphicViewer() = default;

        virtual void renderDisplay( gui::ImGuiWrapper& imGuiWrapper,
                                    std::shared_ptr<core::Environment>& environment ) = 0;

    private:
        void checkAsyncException();

        void renderConsoleArea( gui::ImGuiWrapper& imGuiWrapper,
                                const std::shared_ptr<core::Environment>& environment );

        void
        renderButtons( gui::ImGuiWrapper& imGuiWrapper, const std::shared_ptr<core::Environment>& environment );

        template< typename Function, typename ... Args >
        std::thread thread( Function&& function, Args&& ... args );

        gui::OutputConsoleWidget         console;
        SimpleViewer                     simpleViewer;
        detail::WindowConfigs            windowConfigs;
        std::weak_ptr<core::Environment> environment;
        util::ThreadSafeStringView       runButtonText;
        util::ThreadSafeBool             scrollConsole;
        std::exception_ptr               deferredException;
        std::mutex                       exceptionMutex;
        bool                             firstRender;
        const std::string_view           title;
        bool* const open;
        std::string_view str_id;
    };
}


