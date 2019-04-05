#include "App.hpp"
#include "util/define_logger.hpp"

using aima::core::App;

DEFINE_LOGGER( App );

App::App( aima::gui::ImGuiWrapper& imGuiWrapper ) : imGuiWrapper_( imGuiWrapper ) { TRACE; }

void App::display( bool display ) noexcept {
    TRACE;
    display_ = display;
    LOG4CPLUS_INFO( GetLogger(), "App " << ( display ? "shown" : "hidden" ));
}
