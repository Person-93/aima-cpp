#include <atomic>
#include <csignal>
#include <boost/exception/diagnostic_information.hpp>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include "gui/ImGuiWrapper.hpp"
#include "IntegratedRunner.hpp"
#include "util/configure_logging.hpp"

// IWYU pragma: no_include <string_view>

std::atomic_bool shouldRun = true;

extern "C" void signalHandler( int ) { shouldRun = false; }

int main() try {
    std::signal( SIGTERM, signalHandler );
    aima::util::ConfigureLogging();
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
