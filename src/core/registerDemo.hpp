#pragma once

#include <vector>
#include <functional>
#include <memory>
#include <string_view>
#include <variant>
#include <iostream>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include "util/parseTitle.hpp"
#include "gui/OutputConsoleWidget.hpp"
#include "integrated/forward.hpp"
#include "integrated/IntegratedDemo.hpp"


namespace aima::demos {
    template< typename DemoType >
    void standAloneDemo() {
        static_assert( std::is_base_of_v<core::Demo, DemoType> );
        static_assert( std::is_convertible_v<DemoType*, core::Demo*>, "Must publicly inherit from aima::util::Demo" );
        LOG4CPLUS_INFO( log4cplus::Logger::getRoot(), "Starting standalone demo: " << util::parseTitle<DemoType>());

        DemoType demo( std::cout );
        demo.run();
    }
}

namespace aima::gui { struct ImGuiWrapper; }

namespace aima::IntegratedRunner::detail {
    using IntegratedDemoPtr = std::shared_ptr<IntegratedDemo>;
    using WeakIntegratedDemoPtr = std::weak_ptr<IntegratedDemo>;
    using IntegratedDemoFactory = std::function<IntegratedDemoPtr( gui::ImGuiWrapper&, std::string_view )>;

    class DemoRegistryEntry;

    template< typename DemoType >
    auto registerDemo() {
        using namespace util;
        using namespace gui;
        using aima::core::Demo;

        static_assert( std::is_base_of_v<Demo, DemoType> );
        static_assert( std::is_convertible_v<DemoType*, Demo*>, "Must publicly inherit from aima::util::App" );

        const DemoFactory demoFactory = []( OutputConsoleWidget& consoleWidget ) {
            return std::make_unique<DemoType>( consoleWidget );
        };

        const IntegratedDemoFactory integratedDemoFactory =
                                            [ demoFactory ]( ImGuiWrapper& imGuiWrapper, std::string_view str_id ) {
                                                return std::make_shared<IntegratedDemo>( parseTitle<DemoType>(),
                                                                                         str_id,
                                                                                         imGuiWrapper,
                                                                                         demoFactory );
                                            };

        registerDemo( parseTitle<DemoType>(), integratedDemoFactory );
        return std::monostate{};
    }
}

#ifdef AIMA_STANDALONE_APP

#include <boost/exception/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include "util/configure_logging.hpp"

#define AIMA_REGISTER_DEMO( DemoClass ) \
int main() try {                    \
    using namespace aima::demos;    \
    aima::util::ConfigureLogging(); \
    standAloneDemo<DemoClass>();    \
    LOG4CPLUS_DEBUG( log4cplus::Logger::getRoot(), "Application exiting normally" ); \
    return 0;                       \
}                                   \
catch ( ... ) {                     \
    LOG4CPLUS_FATAL( log4cplus::Logger::getRoot(), boost::current_exception_diagnostic_information( true )); \
    return -1;                      \
}

#elif defined(AIMA_INTEGRATED_APP)

#define AIMA_REGISTER_DEMO( DemoClass ) \
namespace aima::demos {                 \
    static volatile auto m = ::aima::IntegratedRunner::detail::registerDemo<DemoClass>(); \
}

#endif
