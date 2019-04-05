#pragma once

#include <string>
#include <string_view>
#include "core/AppPtr.hpp"
#include "integrated/forward.hpp"


namespace aima::gui { struct ImGuiWrapper; }

namespace aima::IntegratedRunner {
    /**
     * Represents a single registered app
     */
    class AppRegistryEntry {
    public:
        AppRegistryEntry( std::string_view name, AppFactory factory );

        AppRegistryEntry() = delete;

        AppRegistryEntry( const AppRegistryEntry& ) = default;

        AppRegistryEntry( AppRegistryEntry&& ) = default;

        AppRegistryEntry& operator=( const AppRegistryEntry& ) = delete;

        AppRegistryEntry& operator=( AppRegistryEntry&& ) = delete;

        std::string_view name() const noexcept { return name_; }

        AppPtr makeApp( gui::ImGuiWrapper& imGuiWrapper );

        const WeakAppPtr& appPtr() const noexcept { return weakAppPtr; }

    private:
        WeakAppPtr             weakAppPtr;
        AppFactory             factory;
        const std::string_view name_;
        const std::string      str_id;
    };
}
