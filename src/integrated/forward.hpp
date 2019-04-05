#pragma once

#include <memory>       // IWYU pragma: export
#include <functional>
#include <string_view>


namespace aima {
    namespace core { class Demo; }
    namespace gui {
        class OutputConsoleWidget;

        struct ImGuiWrapper;
    }
    namespace IntegratedRunner {
        using DemoPtr = std::unique_ptr<core::Demo>;
        using DemoFactory = std::function<DemoPtr( gui::OutputConsoleWidget& )>;

        class IntegratedDemo;

        using IntegratedDemoPtr = std::shared_ptr<IntegratedDemo>;
        using WeakIntegratedDemoPtr = std::weak_ptr<IntegratedDemo>;
        using IntegratedDemoFactory = std::function<IntegratedDemoPtr( gui::ImGuiWrapper&, std::string_view )>;

        namespace detail {
            void registerDemo( std::string_view name, IntegratedDemoFactory factory );
        }

        class AppPtr;

        class WeakAppPtr;

        using AppFactory = std::function<AppPtr( gui::ImGuiWrapper& )>;

        namespace detail {
            void registerApp( std::string_view name, AppFactory factory );
        }
    }
}
