#pragma once

#include <atomic>
#include <variant>
#include <utility>
#include <string_view>
#include <vector>
#include <functional>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include "util/parseTitle.hpp"
#include "util/Container.hpp"
#include "core/App.hpp"
#include "core/AppPtr.hpp"
#include "gui/ImGuiWrapper.hpp"
#include "integrated/forward.hpp"


namespace aima::gui {
    template< typename AppType >
    void standAloneApp( std::atomic_bool& shouldRun ) {
        static_assert( std::is_base_of_v<aima::core::App, AppType> );
        static_assert( std::is_convertible_v<AppType*, aima::core::App*>,
                       "Must publicly inherit from aima::util::App" );
        LOG4CPLUS_INFO( log4cplus::Logger::getRoot(), "Starting standalone app " << util::parseTitle<AppType>());

        auto         title = util::parseTitle<AppType>();
        ImGuiWrapper imGuiWrapper( title );
        AppType      app( imGuiWrapper );
        while ( shouldRun && !imGuiWrapper.shouldClose() && app.stayOpen()) {
            auto f = imGuiWrapper.frame();
            app.render();
        }
    }
}

namespace aima::IntegratedRunner::detail {
    template< typename AppType >
    auto registerApp() {
        static_assert( std::is_base_of_v<aima::core::App, AppType> );
        static_assert( std::is_convertible_v<AppType*, aima::core::App*>,
                       "Must publicly inherit from aima::util::App" );
        const AppFactory factory = []( gui::ImGuiWrapper& imGuiWrapper ) {
            return makeAppPtr<AppType>( imGuiWrapper );
        };
        registerApp( util::parseTitle<AppType>(), factory );
        return std::monostate{};
    }
}

#ifdef AIMA_STANDALONE_APP

#include <csignal>
#include <iostream>
#include <boost/exception/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include "util/configure_logging.hpp"
#include "util/AssetManager.hpp"
#include "version.hpp"

#define AIMA_REGISTER_APP( AppClass ) \
std::atomic_bool shouldRun = true;    \
extern "C" void handler( int ){ shouldRun = false; } \
int main( int argc, char* argv[] ) try {             \
    std::signal( SIGTERM, handler );  \
    using namespace aima::apps;       \
    aima::util::ConfigureLogging();   \
    LOG4CPLUS_INFO( log4cplus::Logger::getRoot(), "Running aima-cpp version: " << aima::version::longVersion() ); \
    aima::util::AssetManager::setAssetDir( std::filesystem::weakly_canonical( argv[ 0 ] ).parent_path() / "assets" ); \
    aima::gui::standAloneApp<AppClass>( shouldRun ); \
    LOG4CPLUS_DEBUG( log4cplus::Logger::getRoot(), "Application exiting normally" ); \
    return 0;                         \
}                                     \
catch ( ... ) {                       \
    LOG4CPLUS_FATAL( log4cplus::Logger::getRoot(), boost::current_exception_diagnostic_information( true )); \
    return -1;                        \
}

#elif defined(AIMA_INTEGRATED_APP)

#define AIMA_REGISTER_APP( AppClass ) \
namespace aima::apps {                \
    static volatile auto m = ::aima::IntegratedRunner::detail::registerApp<AppClass>(); \
}

#endif
