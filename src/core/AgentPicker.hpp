#include <utility>

#pragma once

#include <memory>
#include <vector>
#include <algorithm>
#include <string>
#include "registerAgent.hpp"
#include "util/parseTitle.hpp"
#include "core/Environment.hpp"

namespace aima::gui {
    struct ImGuiWrapper;
    struct WindowConfig;
}

namespace aima::core {
    namespace detail {
        class AgentPickerBase {
        public:
            bool render( gui::ImGuiWrapper& imGuiWrapper );

            bool done() const { return agentsCreated; }

        protected:
            AgentPickerBase( std::string title, bool* stayOpen );

            virtual void finalize() = 0;

            std::unique_ptr<gui::WindowConfig>         windowConfig;
            std::unordered_map<std::string_view, bool> selections;

        private:
            bool selectionsComplete{ false };
            bool agentsCreated{ false };
        };
    }

    template< typename Base >
    class AgentPicker final : public detail::AgentPickerBase {
    public:
        using Agents = std::vector<std::unique_ptr<Base>>;

        AgentPicker( Environment& environment, bool* stayOpen ) :
                AgentPickerBase{ "Choose " + std::string{ util::parseTitle<Base>() } + "s", stayOpen },
                environment{ environment } {
            auto agentTypes = detail::agentRegistry().at( typeid( Base ));
            for ( auto[name, _]: agentTypes ) {
                selections.insert( { name, false } );
            }
        }

        void finalize() override {
            for ( auto&[name, value] : selections ) {
                if ( value ) {
                    auto p = makeAgent<Base>( name );
                    environment.addAgent( *p );
                    agents.push_back( std::move( p ));
                }
            }
        }

    private:
        Agents agents;
        Environment& environment;
    };
}
