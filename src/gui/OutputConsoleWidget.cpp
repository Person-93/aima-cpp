#include "OutputConsoleWidget.hpp"
#include <utility>
#include "util/define_logger.hpp"
#include "ImGuiWrapper.hpp"

// IWYU pragma: no_include "imgui.h"

using namespace aima::gui;
using namespace aima::util;
using aima::gui::detail::OutputConsoleBuffer;

DEFINE_LOGGER( OutputConsoleWidget );

OutputConsoleBuffer::OutputConsoleBuffer( ThreadSafeBool& outputPastDisplay, ThreadSafeString& display ) :
        std::stringbuf( std::ios_base::out ), outputPastDisplay( &outputPastDisplay ), display( &display ) {
    TRACE;
}

int OutputConsoleBuffer::sync() {
    TRACE;

    *display += str();
    str( "" );
    *outputPastDisplay = true;
    return 0;
}

void OutputConsoleBuffer::clear() {
    TRACE;

    str( "" );
}

OutputConsoleWidget::OutputConsoleWidget()
        : outputPastDisplay( false ), buffer( outputPastDisplay, display ) {
    TRACE;

    rdbuf( &buffer );
    display.access( []( std::string& string ) { string.reserve( 4096 ); } );
}

OutputConsoleWidget::OutputConsoleWidget( OutputConsoleWidget&& other ) noexcept( false ) :
        std::ostream( std::move( other )),
        display( std::move( other.display )),
        outputPastDisplay( std::move( other.outputPastDisplay )),
        buffer( std::move( other.buffer )) {
    TRACE;

    rdbuf( &buffer );
}

OutputConsoleWidget& OutputConsoleWidget::operator=( OutputConsoleWidget&& other ) noexcept( false ) {
    TRACE;

    display           = std::move( other.display );
    buffer            = std::move( other.buffer );
    outputPastDisplay = std::move( other.outputPastDisplay );
    std::ostream::operator=( std::move( other ));
    rdbuf( &buffer );
    return *this;
}

bool OutputConsoleWidget::render( ImGuiWrapper& gui, ChildWindowConfig& config, bool scrollEnabled ) {
    TRACE;

    return gui.childWindow( config, [ this, &scrollEnabled ]() {
        ImGui::TextUnformatted( display.dirtyRead().c_str());
        if ( outputPastDisplay.dirtyRead() && scrollEnabled ) {
            ImGui::SetScrollHereY( 1.0f );
            outputPastDisplay = false;
        }
    } );
}

void OutputConsoleWidget::clear() {
    TRACE;

    display           = "";
    outputPastDisplay = false;
    buffer.clear();
}
