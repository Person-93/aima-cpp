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

    display->access( [ this ]( std::string& string ) {
        string += str();
        if ( string.size() >= 1024 * 1024 ) {
            string.erase( 0, str().size());
        }
    } );
    str( "" );
    *outputPastDisplay = true;
    return 0;
}

void OutputConsoleBuffer::clear() {
    TRACE;

    str( "" );
}

OutputConsoleWidget::OutputConsoleWidget()
        : std::ostream{ new( &bufferStorage ) detail::OutputConsoleBuffer{ outputPastDisplay, display }},
          outputPastDisplay( false ) {
    TRACE;

    display.access( []( std::string& string ) { string.reserve( 4096 ); } );
}

OutputConsoleWidget::OutputConsoleWidget( OutputConsoleWidget&& other ) noexcept :
        std::ostream( std::move( other )),
        display( std::move( other.display )),
        outputPastDisplay( std::move( other.outputPastDisplay )) {
    TRACE;

    using detail::OutputConsoleBuffer;
    new( &bufferStorage ) OutputConsoleBuffer{ std::move( other.buffer()) };
    rdbuf( &buffer());
}

OutputConsoleWidget& OutputConsoleWidget::operator=( OutputConsoleWidget&& other ) noexcept {
    TRACE;

    using detail::OutputConsoleBuffer;
    display           = std::move( other.display );
    outputPastDisplay = std::move( other.outputPastDisplay );
    new( &bufferStorage ) OutputConsoleBuffer{ std::move( other.buffer()) };
    std::ostream::operator=( std::move( other ));
    rdbuf( &buffer());
    return *this;
}

bool OutputConsoleWidget::render( ImGuiWrapper& gui, ChildWindowConfig& config, bool scrollEnabled ) {
    TRACE;

    return gui.childWindow( config, [ this, &scrollEnabled ]() {
        display.sharedAccessDirty( []( std::string_view text ) {
            ImGui::TextUnformatted( text.begin(), text.end());
        } );
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
    buffer().clear();
}
