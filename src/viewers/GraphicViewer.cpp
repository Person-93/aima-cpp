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
using std::endl;
using std::string;

float footerHeightToReserve() {
    // 1 separator, 1 input text
    return GetStyle().ItemSpacing.y + GetFrameHeightWithSpacing();
}

constexpr std::string_view RUN  = "Run";
constexpr std::string_view STOP = "Stop";

GraphicViewer::GraphicViewer( ImGuiWrapper& gui, std::string_view title, bool* open ) :
        console( scrollConsole ),
        simpleViewer( console ),
        windowConfig{ std::make_shared<string>( string( title ) + "##" + util::random_string( 10 )), open },
        consoleAreaConfig{ std::make_shared<string>( string( title ) + "ConsoleSection" ) },
        scrollingSectionConfig{ std::make_shared<string>( string( title ) + "ScrollingSection" ),
                                ImVec2( 0, -footerHeightToReserve()), false },
        runButtonText( RUN ),
        gui( gui ),
        scrollConsole( true ),
        firstRender( false ) {}

void GraphicViewer::notify( std::string_view message ) {
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

void GraphicViewer::setEnvironment( const std::weak_ptr<Environment>& environment ) {
    auto original    = this->environment.lock();
    auto replacement = environment.lock();
    if ( original == replacement ) return;

    this->environment = environment;
    replacement->addEnvironmentView( *this );
    if ( original != nullptr ) console << "The environment changed" << std::endl;
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

void GraphicViewer::renderConsoleArea( std::shared_ptr<Environment>& environment ) {
    gui.childWindow( consoleAreaConfig, [ & ]() {
        auto d = gui.disableControls( environment == nullptr );
        renderButtons( environment );
        Separator();
        console.render( gui, scrollingSectionConfig, scrollConsole.dirtyRead());
    } );
}

void GraphicViewer::renderButtons( std::shared_ptr<Environment>& environment ) {
    if ( SmallButton( "Step" ))
        std::thread( [ this ]( std::shared_ptr<Environment> env ) {
            if ( !env->isRunning())
                env->step();
        }, environment ).detach();
    SameLine();

    if ( SmallButton( runButtonText.dirtyRead().begin()))
        std::thread( [ this ]( std::shared_ptr<Environment> env ) {
            runButtonText = STOP;
            if ( env->isRunning()) env->stop();
            else env->stepUntilDone( 100 );
            runButtonText = RUN;
        }, environment ).detach();
    SameLine();

    if ( SmallButton( "Clear" )) std::thread( [ this ]() { console.clear(); } ).detach();
    SameLine();

    if ( SmallButton( "Scroll" ))
        std::thread( [ this ]() { scrollConsole.access( []( bool& b ) { b = !b; } ); } ).detach();
}
