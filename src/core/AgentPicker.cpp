#include "AgentPicker.hpp"
#include <utility>
#include <thread>
#include "gui/ImGuiWrapper.hpp"

using aima::core::detail::AgentPickerBase;

AgentPickerBase::AgentPickerBase( std::string title, bool* stayOpen ) :
        windowConfig{ std::make_unique<gui::WindowConfig>( gui::WindowConfig{
                .title =  std::move( title ),
                .open  = stayOpen,
        } ) } {}

bool AgentPickerBase::render( aima::gui::ImGuiWrapper& imGuiWrapper ) {
    return imGuiWrapper.window( *windowConfig, [ & ]() {
        for ( auto&[name, value] : selections ) {
            ImGui::Checkbox( name.data(), &value );
        }

        auto disable = imGuiWrapper.disableControls( selectionsComplete );
        if ( ImGui::Button( "OK" )) {
            selectionsComplete = true;
            std::thread( [ this ]() {
                finalize();
                agentsCreated = true;
            } ).detach();
        }
        return true;
    } );
}
