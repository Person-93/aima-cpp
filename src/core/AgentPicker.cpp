#include "AgentPicker.hpp"
#include <utility>
#include <thread>
#include "gui/ImGuiWrapper.hpp"

using aima::core::detail::AgentPickerBase;

AgentPickerBase::AgentPickerBase( std::string title, bool* stayOpen, bool multiSelect ) :
        windowConfig{ std::make_unique<gui::WindowConfig>( gui::WindowConfig{
                .title =  std::move( title ),
                .open  = stayOpen,
        } ) },
        multiSelect{ multiSelect } {}

bool AgentPickerBase::render( aima::gui::ImGuiWrapper& imGuiWrapper ) {
    return imGuiWrapper.window( *windowConfig, [ & ]() {
        const auto onComplete = [ this ] {
            selectionsComplete = true;
            std::thread( [ this ]() {
                finalize();
                agentsCreated = true;
            } ).detach();
        };

        if ( multiSelect ) {
            for ( auto&[name, value] : selections )
                ImGui::Checkbox( name.data(), &value );

            auto disable = imGuiWrapper.disableControls( selectionsComplete );
            if ( ImGui::Button( "OK" ))
                onComplete();
        }
        else
            for ( auto&[name, value] : selections )
                if ( ImGui::Selectable( name.begin(), &value ))
                    onComplete();

        return true;
    } );
}
