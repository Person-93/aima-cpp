#include "imgui.h"
#include "OutputConsoleWidget.hpp"

using namespace aima::gui;

detail::OutputConsoleBuffer::OutputConsoleBuffer( ThreadSafeBool& outputPastDisplay, ThreadSafeString& display ) :
        std::stringbuf( std::ios_base::out ), outputPastDisplay( outputPastDisplay ), display( display ) {}

int detail::OutputConsoleBuffer::sync() {
    display += str();
    str( "" );
    outputPastDisplay = true;
    return 0;
}

void detail::OutputConsoleBuffer::clear() {
    str( "" );
}

OutputConsoleWidget::OutputConsoleWidget( util::ThreadSafeWrapper<bool>& outputPastDisplay )
        : outputPastDisplay( outputPastDisplay ), buffer( outputPastDisplay, display ) {
    rdbuf( &buffer );
}

bool OutputConsoleWidget::render( ImGuiWrapper& gui, ImGuiWrapper::ChildWindowConfig& config, bool scrollEnabled ) {
    return gui.childWindow( config, [ this, &scrollEnabled ]() {
        ImGui::TextUnformatted( display.dirtyRead().c_str());
        if ( outputPastDisplay.dirtyRead() && scrollEnabled ) {
            ImGui::SetScrollHereY( 1.0f );
            outputPastDisplay = false;
        }
    } );
}

void OutputConsoleWidget::clear() {
    display = "";
    buffer.clear();
}
