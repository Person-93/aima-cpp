#include <utility>
#include <thread>

#include "core/Agent.hpp"
#include "core/Environment.hpp"
#include "core/Percept.hpp"
#include "core/Action.hpp"
#include "util/random_string.hpp"
#include "GraphicViewer.hpp"


using namespace aima::core;
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

GraphicViewer::GraphicViewer( ImGuiWrapper& gui, const string& title, bool* open ) :
        console( scrollConsole ),
        simpleViewer( console ),
        windowConfig{ std::make_shared<string>( title + "##" + util::random_string( 10 )), open },
        consoleAreaConfig{ std::make_shared<string>( title + "ConsoleSection" ) },
        scrollingSectionConfig{ std::make_shared<string>( title + "ScrollingSection" ),
                                ImVec2( 0, -footerHeightToReserve()), false },
        runButtonText( RUN ),
        gui( gui ),
        scrollConsole( true ),
        firstRender( false ) {}

void GraphicViewer::notify( std::string_view message ) {
    console << message << "\n" << endl;
    // TODO flashier notification for GraphicViewer ?
}

void GraphicViewer::agentAdded( const Agent& agent, const Environment& source ) {
    console << "Agent " << agent.id() << " added\n" << endl;
}

void GraphicViewer::agentActed( const Agent& agent,
                                const Percept& percept,
                                const Action& action,
                                const Environment& environment ) {
    console << "Agent " << agent.id() << " acted.\n"
            << "Percept: " << percept << "\n"
            << "Action: " << action << "\n"
            << endl;
}

void GraphicViewer::setEnvironment( const std::weak_ptr<Environment>& environment ) {
    auto original    = this->environment.lock();
    auto replacement = environment.lock();
    if ( original == replacement ) return;

    bool wasNull = this->environment.lock() == nullptr;
    this->environment = environment;
    replacement->addEnvironmentView( *this );
    if ( !wasNull ) console << "The environment changed" << std::endl;
}

bool GraphicViewer::render() {
    using namespace ImGui;
    SetNextWindowPos( ImVec2( 20, 20 ), ImGuiCond_Once );
    SetNextWindowSize( ImVec2( 900, 250 ), ImGuiCond_Once );
    return gui.window( windowConfig, [ this ]() {
        auto environment = this->environment.lock();

        Columns( 2 );
        // TODO allow user to resize columns
//        if ( firstRender ) {
        SetColumnWidth( -1, GetWindowWidth() * .4f );
        firstRender = false;
//        }
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

void aima::viewer::GraphicViewer::renderButtons( std::shared_ptr<Environment>& environment ) {
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
