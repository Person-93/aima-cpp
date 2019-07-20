#pragma once

#include <memory>
#include <functional>
#include <type_traits>
#include <unordered_map>
#include <typeindex>
#include <string_view>
#include "util/parseTitle.hpp"

namespace aima::core {
    class Agent;

    namespace detail {
        using AgentRegistry = std::unordered_map<std::type_index,
                                                 std::unordered_map<std::string_view,
                                                                    std::function<std::unique_ptr<Agent>()>>>;

        inline AgentRegistry& agentRegistry() {
            static AgentRegistry registry;
            return registry;
        }
    }

    template< typename Base, typename Derived, typename Factory >
    void registerAgent( Factory factory ) {
        static_assert( std::is_base_of_v<Agent, Base> );
        static_assert( std::is_base_of_v<Base, Derived> );
        static_assert( std::is_same_v<std::unique_ptr<Base>, std::invoke_result_t<Factory>> );

        detail::agentRegistry()[ typeid( Base ) ][ util::parseTitle<Derived>() ] = { factory };
    }

    template< typename Base, typename Derived >
    void registerAgent() {
        registerAgent<Base, Derived>( []() -> std::unique_ptr<Base> {
            return std::make_unique<Derived>();
        } );
    }

    template< typename Base >
    std::unique_ptr<Base> makeAgent( std::string_view type ) {
        static_assert( std::is_base_of_v<Agent, Base> );
        auto p = detail::agentRegistry().at( typeid( Base )).at( type )();
        return std::unique_ptr<Base>{ static_cast<Base*> (p.release()) };
    }
}

#define AIMA_REGISTER_AGENT( Base, Derived )      \
static char dummy = [](){                         \
    ::aima::core::registerAgent<Base, Derived>(); \
    return '\0';                                  \
}()
