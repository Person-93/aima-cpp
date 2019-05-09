#pragma once

#include <memory>
#include <string_view>
#include <exception>
#include <mutex>
#include <thread>
#include <vector>
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

namespace aima::util::AssetManager { class AssetHandle; }

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
        using AssetHandles = std::vector<util::AssetManager::AssetHandle>;
        using AssetLoader  = AssetHandles( * )();

        /**
         * Two phase initialization is required because GraphicViewer object may be crated before the graphics engine
         * is initialized
         * @param imGuiWrapper
         */
        void init( gui::ImGuiWrapper& imGuiWrapper ) {
            if ( initialized ) return;
            initMethod( imGuiWrapper );
            initialized = true;
        }

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

    protected:
        virtual void initMethod( gui::ImGuiWrapper& imGuiWrapper ) {}

        /**
         * Constructs the viewer
         * @param assetLoader A function that loads all assets needed by the viewer
         * @param title The title of the DearImGui window
         * @param open Will be set to false if the user closes the window
         */
        GraphicViewer( AssetLoader assetLoader,
                       std::string_view title,
                       bool* open = nullptr,
                       std::string_view str_id = std::string_view{} );


        /**
         * This method is protected to prevent slicing of derived classes
         */
        ~GraphicViewer();

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
        AssetHandles                     assetHandles;
        bool                             firstRender;
        const std::string_view           title;
        bool* const open;
        std::string str_id;
        bool        initialized = false;
    };
}


