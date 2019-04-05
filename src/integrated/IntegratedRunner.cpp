#include "IntegratedRunner.hpp"
#include <algorithm>
#include "forward.hpp"
#include "IntegratedDemo.hpp"
#include "DemoRegistry.hpp"
#include "AppRegistry.hpp"
#include "gui/ImGuiWrapper.hpp"
#include "core/App.hpp"
#include "core/DemoRegistryEntry.hpp"
#include "core/AppRegistryEntry.hpp"
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

void IntegratedRunner::run( ImGuiWrapper& imGuiWrapper, std::atomic_bool& shouldRun ) {
    TRACE;
    LOG4CPLUS_INFO( GetLogger(), "Running integrated app runner" );

    while ( !imGuiWrapper.shouldClose() && shouldRun) {
        auto f = imGuiWrapper.frame();
        menuBar( imGuiWrapper );

        apps.erase( std::remove_if( apps.begin(), apps.end(),
                                    [ &imGuiWrapper ]( AppPtr app ) {
                                        if ( !app->display() || !app->stayOpen()) return true;
                                        app->render();
                                        return false;
                                    } ),
                    apps.end());

        demos.erase( std::remove_if( demos.begin(), demos.end(),
                                     [ &imGuiWrapper ]( IntegratedDemoPtr demo ) {
                                         if ( !demo->display()) return true;
                                         demo->render();
                                         return false;
                                     } ),
                     demos.end());
    }
}

bool IntegratedRunner::menuBar( ImGuiWrapper& imGuiWrapper ) {
    TRACE;

    return imGuiWrapper.mainMenu( [ & ]() {
        imGuiWrapper.menu( "Demos", !demoRegistry().empty(), [ & ]() {
            for ( auto& entry: demoRegistry()) {
                auto integratedDemo = entry.demoPtr().lock();
                imGuiWrapper.menuItem( entry.name(),
                                       integratedDemo && integratedDemo->display() && integratedDemo->stayOpen(),
                                       !integratedDemo || !integratedDemo->isRunning(),
                                       [ & ]() {
                                           LOG4CPLUS_INFO( GetLogger(), entry.name() << " selected" );
                                           if ( integratedDemo ) integratedDemo->display( !integratedDemo->display());
                                           else demos.emplace_back( entry.makeDemo( imGuiWrapper ));
                                       } );
            }
        } );
        imGuiWrapper.menu( "Apps", !appRegistry().empty(), [ & ]() {
            for ( auto& entry : appRegistry()) {
                auto app = entry.appPtr().lock();
                imGuiWrapper.menuItem( entry.name(), app && app->display() && app->stayOpen(), true,
                                       [ & ]() {
                                           LOG4CPLUS_INFO( GetLogger(), entry.name() << " selected" );
                                           if ( app ) app->display( !app->display());
                                           else apps.emplace_back( entry.makeApp( imGuiWrapper ));
                                       } );
            }
        } );
    } );
}
