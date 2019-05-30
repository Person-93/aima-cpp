#include "IntegratedRunner.hpp"
#include <algorithm>
#include "IntegratedDemo.hpp"
#include "DemoRegistry.hpp"
#include "AppRegistry.hpp"
#include "gui/ImGuiWrapper.hpp"
#include "core/App.hpp"
#include "DemoRegistryEntry.hpp"
#include "AppRegistryEntry.hpp"
#include "util/define_logger.hpp"

using namespace aima::IntegratedRunner::detail;
using aima::IntegratedRunner::IntegratedRunner;
using aima::gui::ImGuiWrapper;

DEFINE_LOGGER( IntegratedRunner );

IntegratedRunner::IntegratedRunner() {
    TRACE;

    apps.reserve( appRegistry().size());
    demos.reserve( demoRegistry().size());
}

IntegratedRunner::~IntegratedRunner() = default;

template< typename T >
static void renderOrErase( std::vector<T>& items ) {
    items.erase( std::remove_if( items.begin(), items.end(),
                                 []( T item ) {
                                     if ( !item->display() || !item->stayOpen()) return true;
                                     item->render();
                                     return false;
                                 } ),
                 items.end());
}

void IntegratedRunner::run( ImGuiWrapper& imGuiWrapper, const std::atomic_bool& shouldRun ) {
    TRACE;
    LOG4CPLUS_INFO( GetLogger(), "Running integrated app runner" );

    while ( !imGuiWrapper.shouldClose() && shouldRun ) {
        auto f = imGuiWrapper.frame();
        menuBar( imGuiWrapper );
        renderOrErase( apps );
        renderOrErase( demos );
    }
}

/**
 * Function for making a drop down menu
 * @param imGuiWrapper A reference to the imgui wrapper
 * @param name The name of the menu
 * @param registry The registry that this menu is for
 * @param windows The container that will hold the window pointers opened by this menu
 * @param enabled A callable that will determine if the menu item is enabled, it should take a window-pointer
 *        and return a bool
 */
template< typename T, typename U, typename Enabled >
static void menu( ImGuiWrapper& imGuiWrapper,
                  std::string_view name,
                  const aima::util::Container<T>& registry,
                  std::vector<U>& windows,
                  Enabled enabled ) {
    TRACE;

    imGuiWrapper.menu( name, !registry.empty(), [ & ]() {
        for ( auto& entry: registry ) {
            auto window = entry.ptr().lock();

            static_assert(
                    std::is_invocable_v<Enabled, std::add_lvalue_reference_t<std::add_const_t<decltype( window )>>> );

            imGuiWrapper.menuItem( entry.name(),
                                   window && window->display() && window->stayOpen(),
                                   enabled( window ),
                                   [ & ]() {
                                       LOG4CPLUS_INFO( GetLogger(), entry.name() << " selected" );
                                       if ( !window ) window = windows.emplace_back( entry.make( imGuiWrapper ));
                                       window->display( !window->display());
                                   } );
        }
    } );
}

void IntegratedRunner::menuBar( ImGuiWrapper& imGuiWrapper ) {
    TRACE;

    imGuiWrapper.mainMenu( [ & ]() -> void {
        menu( imGuiWrapper, "Demos", demoRegistry(), demos,
              []( const IntegratedDemoPtr& w ) { return !w || !w->isRunning(); } );
        menu( imGuiWrapper, "Apps", appRegistry(), apps, []( const AppPtr& ) { return true; } );
    } );
}
