#include "IntegratedDemo.hpp"
#include <thread>
#include <string>
#include <utility>
#include "core/Demo.hpp"
#include "gui/ImGuiWrapper.hpp"
#include "gui/OutputConsoleWidget.hpp"
#include "util/StringBuilder.hpp"
#include "util/define_logger.hpp"


using aima::IntegratedRunner::IntegratedDemo;
using std::string_view;

DEFINE_LOGGER( IntegratedDemo );

namespace aima::IntegratedRunner::detail {
    struct DemoWindowConfigs {
        gui::WindowConfig      parent;
        gui::ChildWindowConfig scroll;
    };
}

namespace {
    using aima::IntegratedRunner::detail::DemoWindowConfigs;

    inline float footerHeightToReserve() {
        // 1 separator, 1 input text
        return ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    }

    std::unique_ptr<DemoWindowConfigs> makeConfig( std::string_view name,
                                                   bool* stayOpen, // NOLINT(readability-non-const-parameter)
                                                   string_view str_id = string_view{} ) {
        using aima::util::StringBuilder;

        return std::make_unique<DemoWindowConfigs>( DemoWindowConfigs{
                .parent = {
                        .title = StringBuilder( 512 ) << name << "##" << str_id,
                        .open  = stayOpen,
                },
                .scroll = {
                        .str_id = StringBuilder( 512 ) << name << "scrolling_section" << str_id,
                        .size   = ImVec2( 0, -footerHeightToReserve()),
                        .border = false,
                },
        } );
    }
}

IntegratedDemo::IntegratedDemo( std::string_view name,
                                std::string_view str_id,
                                gui::ImGuiWrapper& imGuiWrapper,
                                DemoFactory factory ) :
        name( name ),
        str_id( str_id ),
        factory( std::move( factory )),
        imGuiWrapper_( imGuiWrapper ),
        consoleWidget( std::nullopt ),
        windowConfigs( makeConfig( name, &stayOpen_ )),
        stayOpen_( true ) { TRACE; }

void IntegratedDemo::runDemo() {
    TRACE;
    LOG4CPLUS_INFO( GetLogger(), "Running demo: " << name << '-' << str_id );

    std::thread( [ & ]() {
        std::lock_guard lock( mutex );
        consoleWidget.emplace();
        auto demo = factory( *consoleWidget );
        demo->run();
    } ).detach();
}

bool IntegratedDemo::isRunning() const {
    std::unique_lock lock( mutex, std::try_to_lock );
    return !lock;
}

void IntegratedDemo::render() {
    TRACE;
    if ( !consoleWidget.has_value()) {
        LOG4CPLUS_TRACE( GetLogger(), "Rendering demo " << name << '-' << str_id << " skipped" );
        return;
    };
    LOG4CPLUS_TRACE( GetLogger(), "Rendering demo " << name << '-' << str_id );

    imGuiWrapper().window( windowConfigs->parent, [ & ]() {
        consoleWidget->render( imGuiWrapper(), windowConfigs->scroll, true );
    } );
}

bool IntegratedDemo::display() const { return consoleWidget.has_value(); }

void IntegratedDemo::display( bool display ) {
    TRACE;
    if ( display == this->display() || isRunning()) return;

    if ( display ) {
        runDemo();
        LOG4CPLUS_INFO( GetLogger(), "Demo " << name << '-' << str_id << " shown" );
    }
    else {
        consoleWidget = std::nullopt;
        LOG4CPLUS_INFO( GetLogger(), "Demo " << name << '-' << str_id << " hidden" );
    }
}

void aima::IntegratedRunner::IntegratedDemo::setStrId( string_view str_id ) {
    TRACE;
    LOG4CPLUS_DEBUG( GetLogger(), "Demo " << name << ": id=" << str_id );

    windowConfigs = makeConfig( name, &stayOpen_, str_id );
}

IntegratedDemo::~IntegratedDemo() {
    TRACE;
    LOG4CPLUS_INFO( GetLogger(), "Demo " << str_id << " shutting down" );
}
