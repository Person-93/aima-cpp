#include "DemoRegistryEntry.hpp"
#include <utility>
#include "util/random_string.hpp"
#include "util/define_logger.hpp"


using namespace aima::IntegratedRunner::detail;

DEFINE_LOGGER( DemoRegistryEntry );

DemoRegistryEntry::DemoRegistryEntry( std::string_view name, IntegratedDemoFactory factory ) :
        factory( std::move( factory )), name_( name ), str_id( util::random_string( 10 )) {}

aima::IntegratedRunner::IntegratedDemoPtr DemoRegistryEntry::make( gui::ImGuiWrapper& imGuiWrapper ) {
    TRACE;
    LOG4CPLUS_INFO( GetLogger(), "Creating demo from registry entry: \"" << name() << "\" id: " << str_id );

    auto p = factory( imGuiWrapper, str_id );
    demo = p;
    return p;
}
