add_library ( imgui STATIC
              imgui/imgui.cpp
              imgui/imgui_draw.cpp
              imgui/imgui_widgets.cpp
              imgui/examples/imgui_impl_glfw.cpp
              imgui/examples/imgui_impl_opengl3.cpp
              imgui/examples/libs/gl3w/GL/gl3w.c
              )
target_include_directories ( imgui PUBLIC ${CMAKE_CURRENT_LIST_DIR}/imgui
                             ${CMAKE_CURRENT_LIST_DIR}/imgui/examples/libs/gl3w
                             )
target_link_libraries ( imgui INTERFACE glfw )
if ( UNIX AND NOT APPLE )
    target_link_libraries ( imgui PRIVATE dl )
else ()
    message ( FATAL_ERROR "Imgui dependencies were not configured for this platform" )
endif ()

add_executable ( imgui_demo imgui/examples/example_glfw_opengl3/main.cpp imgui/imgui_demo.cpp )
target_link_libraries ( imgui_demo PRIVATE imgui )
target_include_directories ( imgui_demo PRIVATE imgui/examples )
