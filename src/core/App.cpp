#include "App.hpp"
#include "util/define_logger.hpp"
#include "views/GraphicViewer.hpp"

using aima::core::App;

DEFINE_LOGGER( App );

App::App( aima::gui::ImGuiWrapper& imGuiWrapper ) : imGuiWrapper_( imGuiWrapper ) { TRACE; }

void App::display( bool display ) noexcept {
    TRACE;
    viewer().init( imGuiWrapper());
    display_ = display;
    LOG4CPLUS_INFO( GetLogger(), "App " << ( display ? "shown" : "hidden" ));
}
