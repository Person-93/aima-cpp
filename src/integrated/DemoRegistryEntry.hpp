#pragma once

#include <string_view>
#include <string>
#include "integrated/forward.hpp"

namespace aima::gui { struct ImGuiWrapper; }
namespace aima::IntegratedRunner::detail {
    /**
     * Represents a single registered demo
     */
    class DemoRegistryEntry {
    public:
        explicit DemoRegistryEntry( std::string_view name, IntegratedDemoFactory factory );

        std::string_view name() const noexcept { return name_; }

        IntegratedDemoPtr make( gui::ImGuiWrapper& imGuiWrapper );

        const WeakIntegratedDemoPtr& ptr() const noexcept { return demo; }

        ~DemoRegistryEntry() = default;

    private:
        const IntegratedDemoFactory factory;
        const std::string           str_id;
        const std::string_view      name_;
        WeakIntegratedDemoPtr       demo;
    };
}
