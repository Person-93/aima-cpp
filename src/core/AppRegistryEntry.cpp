#include "AppRegistryEntry.hpp"
#include <functional>
#include <utility>
#include "util/define_logger.hpp"
#include "util/random_string.hpp"


using namespace aima::IntegratedRunner;
using aima::IntegratedRunner::AppPtr;

DEFINE_LOGGER( AppRegistryEntry );

AppRegistryEntry::AppRegistryEntry( std::string_view name, AppFactory factory ) :
        name_( name ), factory( std::move( factory )), str_id( util::random_string( 10 )) {}

AppPtr AppRegistryEntry::makeApp( aima::gui::ImGuiWrapper& imGuiWrapper ) {
    TRACE;
    LOG4CPLUS_INFO( GetLogger(), "Creating app from registry entry: \"" << name() << "\" id: " << str_id );

    auto p = factory( imGuiWrapper );
    weakAppPtr = p;
    return p;
}


