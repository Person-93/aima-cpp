#include "GraphicViewer.hpp"
#include <thread>
#include <string>
#include <string_view>
#include <functional>
#include <utility>
#include <boost/exception/diagnostic_information.hpp>
#include "core/Environment.hpp"
#include "util/StringBuilder.hpp"
#include "util/define_logger.hpp"
#include "util/AssetManager.hpp"
#include "util/random_string.hpp"

using namespace aima::core;
using namespace aima::gui;
using namespace ImGui;
using aima::viewer::GraphicViewer;
using std::string;
using std::string_view;
using std::shared_ptr;
using std::weak_ptr;
using std::make_shared;

DEFINE_LOGGER( GraphicViewer );

#define LOG_GUI_EVENT( event ) LOG4CPLUS_DEBUG( GetLogger(), "Viewer-id: " << this->str_id << " Event: " << event )
#define LOG_BUTTON_PRESS( button ) LOG_GUI_EVENT( button << " button pressed" )

namespace {
    using aima::viewer::detail::WindowConfigs;
    using aima::util::StringBuilder;

    inline float footerHeightToReserve() {
        // 1 separator, 1 input text
        return GetStyle().ItemSpacing.y + GetFrameHeightWithSpacing();
    }

    inline WindowConfigs makeWindowConfigs( string_view title,
                                            bool* open, // NOLINT(readability-non-const-parameter)
                                            std::string_view str_id ) {
        return WindowConfigs{
                .windowConfig = {
                        .title = StringBuilder( 512 ) << title << "##" << str_id,
                        .open  = open,
                },
                .consoleAreaConfig = {
                        .str_id = StringBuilder( 512 ) << title << "ConsoleSection##" << str_id,
                },
                .scrollingSectionConfig = {
                        .str_id = StringBuilder( 512 ) << title << "ScrollingSection##" << str_id,
                        .size   = ImVec2( 0, -footerHeightToReserve()),
                        .border = false,
                },
        };
    }
}

constexpr std::string_view RUN  = "Run";
constexpr std::string_view STOP = "Stop";

GraphicViewer::GraphicViewer( AssetLoader assetLoader,
                              std::string_view title,
                              bool* open,
                              std::string_view str_id ) :
        console(),
        simpleViewer( console ),
        windowConfigs( makeWindowConfigs( title, open, str_id )),
        runButtonText( RUN ),
        scrollConsole( true ),
        assetHandles( assetLoader ? assetLoader() : []() -> AssetHandles {
            using namespace aima::core::exception;
            AIMA_THROW_EXCEPTION( Exception{} << Because( "Graphic viewer received null asset loader" ));
        }()),
        firstRender( false ),
        title( title ),
        open( open ),
        str_id( str_id.empty() ? util::random_string( 10 ) : str_id ) {
    TRACE;
    LOG_GUI_EVENT( "Created" );
}

void GraphicViewer::notify( string_view message ) {
    TRACE;
    simpleViewer.notify( message );
    // TODO flashier notification for GraphicViewer ?
}

void GraphicViewer::agentAdded( const Agent& agent, const Environment& source ) {
    TRACE;
    simpleViewer.agentAdded( agent, source );
}

void GraphicViewer::agentActed( const Agent& agent,
                                const Percept& percept,
                                const Action& action,
                                const Environment& environment ) {
    TRACE;
    simpleViewer.agentActed( agent, percept, action, environment );
}

void GraphicViewer::setEnvironment( const shared_ptr<Environment>& environment ) {
    TRACE;

    auto original = this->environment.lock();
    if ( original == environment ) return;

    this->environment = environment;

    if ( original ) {
        original->removeEnvironmentView( *this );
        console << "The environment changed" << std::endl;
    }

    if ( environment ) environment->addEnvironmentView( *this );
}

bool GraphicViewer::render( gui::ImGuiWrapper& imGuiWrapper ) {
    TRACE;
    if ( !initialized ) {
        // throw instead of just initializing because initialization should not happen in the render loop
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << Because( "GraphicViewer cannot be rendered before init() is called" ));
    }
    checkAsyncException();

    using namespace ImGui;
    SetNextWindowPos( ImVec2( 20, 20 ), ImGuiCond_Once );
    SetNextWindowSize( ImVec2( 900, 250 ), ImGuiCond_Once );
    return imGuiWrapper.window( windowConfigs.windowConfig, [ & ]() {
        auto environment = this->environment.lock();

        Columns( 2 );
        SetColumnWidth( -1, GetWindowWidth() * .4f );
        firstRender = false;
        renderConsoleArea( imGuiWrapper, environment );
        NextColumn();
        renderDisplay( imGuiWrapper, environment );
        Columns( 1 );
    } );
}

GraphicViewer::~GraphicViewer() {
    TRACE;
    LOG_GUI_EVENT( "Destroyed" );
}

void GraphicViewer::renderConsoleArea( ImGuiWrapper& imGuiWrapper, const std::shared_ptr<Environment>& environment ) {
    TRACE;
    imGuiWrapper.childWindow( windowConfigs.consoleAreaConfig, [ & ]() {
        auto d = imGuiWrapper.disableControls( environment == nullptr );
        renderButtons( imGuiWrapper, environment );
        Separator();
        console.render( imGuiWrapper, windowConfigs.scrollingSectionConfig, scrollConsole.dirtyRead());
    } );
}

void GraphicViewer::renderButtons( ImGuiWrapper& imGuiWrapper, const std::shared_ptr<Environment>& environment ) {
    TRACE;
    if ( constexpr string_view buttonText = "Step"; SmallButton( buttonText.data()))
        thread( [ this, buttonText ]( shared_ptr<Environment> env ) {
            LOG_BUTTON_PRESS( buttonText.data());
            if ( env && !env->isRunning()) {
                LOG_BUTTON_PRESS( "Stepping" );
                env->step();
            }
        }, environment )
                .detach();
    SameLine();

    if ( const string_view buttonText = runButtonText.dirtyRead(); SmallButton( buttonText.begin()))
        thread( [ this, buttonText ]( shared_ptr<Environment> env ) {
            LOG_BUTTON_PRESS( buttonText );
            if ( !env ) return;
            runButtonText = STOP;
            if ( env->isRunning()) {
                LOG_GUI_EVENT( "Stopping" );
                env->stop();
            }
            else {
                LOG_GUI_EVENT( "Running" );
                env->stepUntilDone();
            }
            runButtonText = RUN;
        }, environment )
                .detach();
    SameLine();

    if ( constexpr string_view buttonText = "Clear"; SmallButton( buttonText.begin()))
        thread( [ this, buttonText ]() {
            LOG_BUTTON_PRESS( buttonText );
            LOG_GUI_EVENT( "Clearing Console" );
            console.clear();
        } )
                .detach();
    SameLine();

    if ( constexpr string_view buttonText = "Scroll"; SmallButton( buttonText.begin()))
        thread( [ this, buttonText ]() {
            LOG_BUTTON_PRESS( buttonText );
            LOG_GUI_EVENT( "Toggling console scroll" );
            scrollConsole.access( []( bool& b ) { b = !b; } );
        } )
                .detach();
}

void GraphicViewer::checkAsyncException() {
    TRACE;
    std::unique_lock lock( exceptionMutex, std::try_to_lock );
    if ( !lock || !deferredException ) return;

    LOG4CPLUS_DEBUG( GetLogger(), "Handling asynchronous exception" );
    try {
        std::rethrow_exception( deferredException );
    }
    catch ( ... ) {
        deferredException = nullptr;

        std::thread( [ &console = this->console ]( std::exception_ptr exception ) {
            try {
                std::rethrow_exception( exception );
            }
            catch ( ... ) {
                LOG4CPLUS_ERROR( GetLogger(), boost::current_exception_diagnostic_information());
                console << std::endl << boost::current_exception_diagnostic_information() << std::endl;
            }
        }, std::current_exception())
                .detach();
    }
}

template< typename Function, typename... Args >
std::thread GraphicViewer::thread( Function&& function, Args&& ... args ) {
    TRACE;

    using namespace std;
    static_assert( is_invocable_v<Function, Args...> );

    constexpr auto wrapper =
                           []( mutex& exceptionMutex,
                               exception_ptr& exceptionPtr,
                               Function&& f,
                               Args&& ... args ) {
                               LOG4CPLUS_TRACE( GetLogger(), "Started async operation" );

                               {
                                   unique_lock lock( exceptionMutex, try_to_lock );
                                   if ( !lock || exceptionPtr ) {
                                       LOG4CPLUS_TRACE( GetLogger(), "Async operation exited early" );
                                       return;
                                   }
                               }

                               try {
                                   forward<Function>( f )( forward<Args>( args )... );
                                   LOG4CPLUS_TRACE( GetLogger(), "Async operation succeeded" );
                               }
                               catch ( ... ) {
                                   LOG4CPLUS_TRACE( GetLogger(), "Async operation failed" );
                                   unique_lock lock( exceptionMutex );
                                   if ( !exceptionPtr ) exceptionPtr = current_exception();
                               }
                           };

    return ::std::thread( wrapper,
                          ref( exceptionMutex ),
                          ref( deferredException ),
                          forward<Function>( function ),
                          forward<Args>( args )... );
}
