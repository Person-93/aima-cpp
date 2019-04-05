#include "ImGuiWrapper.hpp" // IWYU pragma: associated
#include <GL/glcorearb.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "core/Exception.hpp"
#include "imgui_internal.h"
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>
#include "util/define_logger.hpp"


class ChildWindow;
class MainMenu;
class Menu;
class Window;

using namespace aima::gui;

DEFINE_LOGGER( ImGuiWrapper );

GLFWwindow* glfwWindow = nullptr;
constexpr int width  = 1280;
constexpr int height = 720;
ImVec4* clear_color = nullptr;

static struct {
    int error;
    const char* description;
} err;

ImGuiWrapper::ImGuiWrapper( std::string_view title ) {
    TRACE;

    if ( glfwWindow ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << Because( "There can only be one instance of ImGuiWrapper" ));
    }
    if ( !clear_color ) clear_color = new ImVec4{ 0.45f, 0.55f, 0.60f, 1.00f };

    // Setup window
    glfwSetErrorCallback( []( int error, const char* description ) {
        err.error       = error;
        err.description = description;
    } );

    if ( !glfwInit()) {
        std::ostringstream ss;
        ss << "GLFW Error " << err.error << ": " << err.description;

        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << Because( ss.str()));
    }

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint( GLFW_SAMPLES, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
    glfwWindowHint( GLFW_MAXIMIZED, GLFW_TRUE );

    // Create window with graphics context
    glfwWindow = glfwCreateWindow( width, height, title.begin(), nullptr, nullptr );
    if ( glfwWindow == nullptr ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << Because( "Unable to create GLFW window" ));
    }

    glfwMakeContextCurrent( glfwWindow );
    glfwSwapInterval( 1 ); // Enable vsync

    if ( gl3wInit()) {
        static struct {
            int major, minor;
        } version;
        glGetIntegerv( GL_MAJOR_VERSION, &version.major );
        glGetIntegerv( GL_MINOR_VERSION, &version.minor );
        std::ostringstream ss;
        ss << "Failed to initialize OpenGL loader. Version is: " << version.major << '.' << version.minor;

        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << Because( ss.str()));
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL( glfwWindow, true );
    ImGui_ImplOpenGL3_Init( glsl_version );
}

ImGuiWrapper::~ImGuiWrapper() {
    TRACE;

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow( glfwWindow );
    glfwTerminate();
    glfwWindow = nullptr;
}

ImGuiWrapper::Frame ImGuiWrapper::frame() {
    TRACE;
    return ImGuiWrapper::Frame();
}

bool inFrame = false;

ImGuiWrapper::Frame::Frame() {
    TRACE;

    if ( inFrame ) {
        using namespace aima::core::exception;
        AIMA_THROW_EXCEPTION( Exception{} << Because( "Cannot start new frame until previous frame is completed" ));
    }
    inFrame = true;

    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

ImGuiWrapper::Frame::~Frame() {
    TRACE;

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwMakeContextCurrent( glfwWindow );
    glfwGetFramebufferSize( glfwWindow, &display_w, &display_h );
    glViewport( 0, 0, display_w, display_h );
    glClearColor( clear_color->x, clear_color->y, clear_color->z, clear_color->w );
    glClear( GL_COLOR_BUFFER_BIT );
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData());

    glfwMakeContextCurrent( glfwWindow );
    glfwSwapBuffers( glfwWindow );

    inFrame = false;
}

#define WIDGET( widget, close, AlwaysClose ) \
class widget {                               \
public:                                      \
    CONSTRUCTORS                             \
    ~widget() {                              \
    TRACE;                                   \
        if( AlwaysClose || opened ) close(); \
    }                                        \
    operator bool() { return opened; }       \
private:                                     \
    bool opened;                             \
}

#define CONSTRUCTORS Window(std::string_view title, bool* open, ImGuiWindowFlags flags) { \
    TRACE;                                             \
    opened = ImGui::Begin(title.begin(), open, flags); \
}

WIDGET( Window, ImGui::End, true );

#undef CONSTRUCTORS

bool ImGuiWrapper::window( WindowConfig& config, std::function<void()> function ) {
    TRACE;

    Window w( config.title.c_str(), config.open, config.flags );
    if ( w ) function();
    return w;
}

bool ImGuiWrapper::shouldClose() {
    TRACE;
    return bool( glfwWindowShouldClose( glfwWindow ));
}

#define CONSTRUCTORS ChildWindow(std::string_view str_id, const ImVec2& size, bool border, ImGuiWindowFlags flags) { \
    TRACE;\
    opened = ImGui::BeginChild( str_id.begin(), size, border, flags ); \
}

WIDGET( ChildWindow, ImGui::EndChild, true );

#undef CONSTRUCTORS

bool ImGuiWrapper::childWindow( ChildWindowConfig& config, std::function<void()> function ) {
    TRACE;

    ChildWindow w( config.str_id.c_str(), config.size, config.border, config.flags );
    if ( w ) function();
    return w;
}

void ImGuiWrapper::setWindowTitle( std::string_view title ) {
    TRACE;
    LOG4CPLUS_INFO( GetLogger(), "Window title was set to " << title );
    glfwSetWindowTitle( glfwWindow, title.begin());
}

ImGuiWrapper::DisableControls ImGuiWrapper::disableControls( bool disable ) {
    TRACE;
    return ImGuiWrapper::DisableControls( disable );
}

#define CONSTRUCTORS MainMenu() {       \
    TRACE;                              \
    opened = ImGui::BeginMainMenuBar(); \
}

WIDGET( MainMenu, ImGui::EndMainMenuBar, false );

bool ImGuiWrapper::mainMenu( std::function<void()> function ) {
    TRACE;
    MainMenu m;
    if ( m ) function();
    return m;
}

#undef CONSTRUCTORS
#define CONSTRUCTORS Menu( std::string_view label, bool enabled ) { \
    TRACE;                                                          \
    opened = ImGui::BeginMenu( label.begin(), enabled );            \
}

WIDGET( Menu, ImGui::EndMenu, false );

void ImGuiWrapper::menu( std::string_view label, bool enabled, std::function<void()> function ) {
    TRACE;
    Menu m( label, enabled );
    if ( m ) function();
}

#undef CONSTRUCTORS
#undef WIDGET

void ImGuiWrapper::menuItem( std::string_view label, bool selected, bool enabled, std::function<void()> function ) {
    TRACE;
    if ( ImGui::MenuItem( label.begin(), nullptr, selected, enabled )) function();
}

ImGuiWrapper::DisableControls::DisableControls( bool disable ) : disabled( false ) {
    TRACE;
    if ( disable ) this->disable();
}

ImGuiWrapper::DisableControls::~DisableControls() {
    TRACE;
    enable();
}

void ImGuiWrapper::DisableControls::disable() {
    TRACE;
    if ( !disabled ) {
        ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
        ImGui::PushStyleVar( ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f );
        disabled = true;
    }
}

void ImGuiWrapper::DisableControls::enable() {
    TRACE;
    if ( disabled ) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
        disabled = false;
    }
}

#include "imgui_config.hpp"

log4cplus::Logger& aima::gui::detail::GetLogger() {
    return ::GetLogger();
}
