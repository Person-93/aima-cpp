#include <atomic>
#include <csignal>
#include <boost/exception/diagnostic_information.hpp>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <version.hpp>
#include "gui/ImGuiWrapper.hpp"
#include "IntegratedRunner.hpp"
#include "util/configure_logging.hpp"
#include "util/AssetManager.hpp"

// IWYU pragma: no_include <string_view>

std::atomic_bool shouldRun = true;

extern "C" void signalHandler( int ) { shouldRun = false; }

int main( int argc, char* argv[] ) try {
    std::signal( SIGTERM, signalHandler );
    aima::util::ConfigureLogging();
    LOG4CPLUS_INFO( log4cplus::Logger::getRoot(), "Running aima-cpp version: " << aima::version::longVersion());
    aima::util::AssetManager::setAssetDir( std::filesystem::weakly_canonical( argv[ 0 ] ).parent_path() / "assets" );
    aima::IntegratedRunner::IntegratedRunner integratedRunner;
    aima::gui::ImGuiWrapper                  imGuiWrapper( "AIMA Integrated App Runner" );
    integratedRunner.run( imGuiWrapper, shouldRun );
    LOG4CPLUS_DEBUG( log4cplus::Logger::getRoot(), "Application exiting normally" );
    return 0;
}
catch ( ... ) {
    LOG4CPLUS_FATAL( log4cplus::Logger::getRoot(), boost::current_exception_diagnostic_information( true ));
    return -1;
}
