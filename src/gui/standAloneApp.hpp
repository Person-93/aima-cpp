#pragma once

#include "util/parseTitle.hpp"
#include "ImGuiWrapper.hpp"

namespace aima::gui {
    template< typename App >
    void standAloneApp() {
        static_assert( std::is_base_of_v<aima::util::App, App> );
        static_assert( std::is_convertible_v<App*, aima::util::App*>, "Must publicly inherit from aima::util::App" );
        auto         title    = util::parseTitle<App>();
        ImGuiWrapper gui( title );
        bool         stayOpen = true;
        App          app( gui, &stayOpen );
        while ( !gui.shouldClose() && stayOpen ) {
            auto f = gui.frame();
            app.render();
        }
    }
}
