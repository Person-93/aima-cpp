#include "App.hpp"
#include "util/define_logger.hpp"
#include "views/GraphicViewer.hpp"

using aima::core::App;

DEFINE_LOGGER( App );

App::App( aima::gui::ImGuiWrapper& imGuiWrapper ) : imGuiWrapper_( imGuiWrapper ) {
    TRACE;
    LOG4CPLUS_INFO( GetLogger(), "App " << id() << " created" );
}

void App::display( bool display ) noexcept {
    TRACE;
    if ( display_ == display ) return;
    display_ = display;
    if ( display ) viewer().init( imGuiWrapper());
    LOG4CPLUS_INFO( GetLogger(), "App " << id() << ' ' << ( display ? "shown" : "hidden" ));
}

App::~App() {
    TRACE;
    LOG4CPLUS_INFO( GetLogger(), "App " << id() << " shutting down" );
}
