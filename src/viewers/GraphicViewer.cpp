#include <thread>
#include <ostream>
#include <string>

#include "core/Environment.hpp"
#include "util/random_string.hpp"
#include "GraphicViewer.hpp" // IWYU pragma: associated


using namespace aima::core;
using namespace aima::gui;
using namespace ImGui;
using aima::viewer::GraphicViewer;
using std::string;
using std::string_view;
using std::shared_ptr;
using std::weak_ptr;
using std::make_shared;

float footerHeightToReserve() {
    // 1 separator, 1 input text
    return GetStyle().ItemSpacing.y + GetFrameHeightWithSpacing();
}

constexpr std::string_view RUN  = "Run";
constexpr std::string_view STOP = "Stop";

GraphicViewer::GraphicViewer( ImGuiWrapper& gui, string_view title, bool* open ) :
        console( scrollConsole ),
        simpleViewer( console ),
        windowConfig{ make_shared<string>( string( title ) + "##" + util::random_string( 10 )), open },
        consoleAreaConfig{ make_shared<string>( string( title ) + "ConsoleSection" ) },
        scrollingSectionConfig{ make_shared<string>( string( title ) + "ScrollingSection" ),
                                ImVec2( 0, -footerHeightToReserve()), false },
        runButtonText( RUN ),
        gui( gui ),
        scrollConsole( true ),
        firstRender( false ) {}

void GraphicViewer::notify( string_view message ) {
    simpleViewer.notify( message );
    // TODO flashier notification for GraphicViewer ?
}

void GraphicViewer::agentAdded( const Agent& agent, const Environment& source ) {
    simpleViewer.agentAdded( agent, source );
}

void GraphicViewer::agentActed( const Agent& agent,
                                const Percept& percept,
                                const Action& action,
                                const Environment& environment ) {
    simpleViewer.agentActed( agent, percept, action, environment );
}

void GraphicViewer::setEnvironment( const shared_ptr<Environment>& environment ) {
    auto original = this->environment.lock();
    if ( original == environment ) return;

    this->environment = environment;
    if ( original ) {
        original->removeEnvironmentView( *this );
        console << "The environment changed" << std::endl;
    }
    environment->addEnvironmentView( *this );
}

bool GraphicViewer::render() {
    using namespace ImGui;
    SetNextWindowPos( ImVec2( 20, 20 ), ImGuiCond_Once );
    SetNextWindowSize( ImVec2( 900, 250 ), ImGuiCond_Once );
    return gui.window( windowConfig, [ this ]() {
        auto environment = this->environment.lock();

        Columns( 2 );
        SetColumnWidth( -1, GetWindowWidth() * .4f );
        firstRender = false;
        renderConsoleArea( environment );
        NextColumn();
        renderDisplay( environment );
        Columns( 1 );
    } );
}

void GraphicViewer::renderConsoleArea( const shared_ptr<Environment>& environment ) {
    gui.childWindow( consoleAreaConfig, [ & ]() {
        auto d = gui.disableControls( environment == nullptr );
        renderButtons( environment );
        Separator();
        console.render( gui, scrollingSectionConfig, scrollConsole.dirtyRead());
    } );
}

void GraphicViewer::renderButtons( const shared_ptr<Environment>& environment ) {
    using std::thread;

    if ( SmallButton( "Step" ))
        thread( []( shared_ptr<Environment> env ) {
            if ( env && !env->isRunning())
                env->step();
        }, environment ).detach();
    SameLine();

    if ( SmallButton( runButtonText.dirtyRead().begin()))
        thread( [ &runButtonText = this->runButtonText ]( shared_ptr<Environment> env ) {
            if ( !env ) return;
            runButtonText = STOP;
            if ( env->isRunning()) env->stop();
            else env->stepUntilDone( 100 );
            runButtonText = RUN;
        }, environment ).detach();
    SameLine();

    if ( SmallButton( "Clear" )) thread( [ &console = this->console ]() { console.clear(); } ).detach();
    SameLine();

    if ( SmallButton( "Scroll" ))
        thread( [ &scrollConsole = this->scrollConsole ]() {
            scrollConsole.access( []( bool& b ) { b = !b; } );
        } ).detach();
}
