#pragma once

#include <string_view>

#include "core/EnvironmentView.hpp"
#include "gui/ImGuiWrapper.hpp"
#include "gui/OutputConsoleWidget.hpp"
#include "util/ThreadSafeWrapper.hpp"
#include "SimpleViewer.hpp"

namespace aima::core {
    class Agent;

    class Percept;

    class Action;
}

namespace aima::viewer {
    namespace {
        using namespace aima::core;
        using namespace aima::gui;
    }
    /**
     * A viewer that displays a graphic representation of the environment to the user in a DearImGui window
     */
    class GraphicViewer : public EnvironmentView {
    public:
        /**
         * Constructs the viewer
         * @param gui The instance of ImGuiWrapper
         * @param title The title of the DearImGui window
         * @param open Will be set to false if the user closes the window
         */
        GraphicViewer( ImGuiWrapper& gui, const std::string& title, bool* open = nullptr );

        void notify( std::string_view message ) override;

        void agentAdded( const Agent& agent, const Environment& source ) override;

        void agentActed( const Agent& agent,
                         const Percept& percept,
                         const Action& action,
                         const Environment& environment ) override;

        virtual void setEnvironment( const std::weak_ptr<Environment>& environment );

        bool render();

    protected:
        virtual void renderDisplay( std::shared_ptr<Environment>& environment ) = 0;

    private:
        void renderConsoleArea( std::shared_ptr<Environment>& environment );

        void renderButtons( std::shared_ptr<Environment>& environment );

        OutputConsoleWidget                       console;
        SimpleViewer                              simpleViewer;
        ImGuiWrapper::WindowConfig                windowConfig;
        ImGuiWrapper::ChildWindowConfig           consoleAreaConfig;
        ImGuiWrapper::ChildWindowConfig           scrollingSectionConfig;
        std::weak_ptr<Environment>                environment;
        util::ThreadSafeWrapper<std::string_view> runButtonText;
        ImGuiWrapper& gui;
        util::ThreadSafeWrapper<bool> scrollConsole;
        bool                          firstRender;
    };
}


