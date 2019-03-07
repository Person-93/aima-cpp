#pragma once

#include <memory>
#include <string_view>

#include "core/EnvironmentView.hpp"
#include "gui/ImGuiWrapper.hpp"
#include "gui/OutputConsoleWidget.hpp"
#include "util/ThreadSafeWrapper.hpp"
#include "viewers/SimpleViewer.hpp"

namespace aima::core {
    class Agent;

    class Percept;

    class Action;

    class Environment;
}

namespace aima::viewer {
    /**
     * A viewer that displays a graphic representation of the environment to the user in a DearImGui window
     */
    class GraphicViewer : public core::EnvironmentView {
    public:
        /**
         * Constructs the viewer
         * @param gui The instance of ImGuiWrapper
         * @param title The title of the DearImGui window
         * @param open Will be set to false if the user closes the window
         */
        GraphicViewer( gui::ImGuiWrapper& gui, std::string_view title, bool* open = nullptr );

        void notify( std::string_view message ) override;

        void agentAdded( const core::Agent& agent, const core::Environment& source ) override;

        void agentActed( const core::Agent& agent,
                         const core::Percept& percept,
                         const core::Action& action,
                         const core::Environment& environment ) override;

        virtual void setEnvironment( const std::shared_ptr<core::Environment>& environment );

        bool render();

    protected:
        virtual void renderDisplay( std::shared_ptr<core::Environment>& environment ) = 0;

    private:
        void renderConsoleArea( const std::shared_ptr<core::Environment>& environment );

        void renderButtons( const std::shared_ptr<core::Environment>& environment );

        gui::OutputConsoleWidget                  console;
        SimpleViewer                              simpleViewer;
        gui::ImGuiWrapper::WindowConfig           windowConfig;
        gui::ImGuiWrapper::ChildWindowConfig      consoleAreaConfig;
        gui::ImGuiWrapper::ChildWindowConfig      scrollingSectionConfig;
        std::weak_ptr<core::Environment>          environment;
        util::ThreadSafeWrapper<std::string_view> runButtonText;
        gui::ImGuiWrapper& gui;
        util::ThreadSafeWrapper<bool> scrollConsole;
        bool                          firstRender;
    };
}


