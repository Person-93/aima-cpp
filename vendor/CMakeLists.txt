add_library ( imgui_base STATIC
              imgui/imgui.h
              imgui/imgui.cpp
              imgui/imgui_draw.cpp
              imgui/imgui_widgets.cpp
              imgui/examples/imgui_impl_glfw.cpp
              imgui/examples/imgui_impl_glfw.h
              imgui/examples/imgui_impl_opengl3.cpp
              imgui/examples/imgui_impl_opengl3.h
              imgui/examples/libs/gl3w/GL/gl3w.c
              imgui/examples/libs/gl3w/GL/gl3w.h
              imgui/examples/libs/gl3w/GL/glcorearb.h
              )
target_include_directories ( imgui_base PUBLIC
                             ${CMAKE_CURRENT_LIST_DIR}/imgui
                             ${CMAKE_CURRENT_LIST_DIR}/imgui/examples/libs/gl3w
                             )
target_include_directories ( imgui_base PRIVATE ${PROJECT_SOURCE_DIR}/src )
target_link_libraries ( imgui_base INTERFACE glfw )
if ( UNIX AND NOT APPLE )
    target_link_libraries ( imgui_base PRIVATE dl )
else ()
    message ( FATAL_ERROR "Imgui dependencies were not configured for this platform" )
endif ()
if ( BUILD_SHARED_LIBS )
    set_property ( TARGET imgui_base PROPERTY POSITION_INDEPENDENT_CODE ON )
endif ()

add_executable ( imgui_demo
                 imgui/imgui_demo.cpp
                 imgui/examples/example_glfw_opengl3/main.cpp
                 )
target_link_libraries ( imgui_demo PRIVATE imgui_base )
target_include_directories ( imgui_demo PRIVATE imgui/examples )

add_library ( stb INTERFACE )
target_include_directories ( stb INTERFACE stb )

add_library ( SQLite INTERFACE )
target_include_directories ( SQLite INTERFACE sqlite_modern_cpp/hdr ${SQLite3_INCLUDE_DIRS} )
target_link_libraries ( SQLite INTERFACE ${SQLite3_LIBRARIES} )

message ( STATUS "Building cppcoro" )
set ( CPPCORO_SRC ${CMAKE_CURRENT_BINARY_DIR}/cppcoro )
file ( COPY cppcoro DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
execute_process ( COMMAND python2.7 ${CPPCORO_SRC}/tools/cake/src/run.py --clang-executable=${CMAKE_CXX_COMPILER} ${CPPCORO_SRC}/lib/build.cake
                  RESULT_VARIABLE result
                  OUTPUT_VARIABLE output
                  ERROR_VARIABLE output
                  )
message ( "${output}" )
if ( NOT result EQUAL 0 )
    message ( FATAL_ERROR "Unable to build cppcoro" )
endif ()
add_library ( cppcoro INTERFACE )
target_include_directories ( cppcoro INTERFACE cppcoro/include )
target_link_libraries ( cppcoro INTERFACE ${CPPCORO_SRC}/build/linux_x64_clang7.0.1_debug/lib/libcppcoro.a )

set ( CMAKE_POLICY_DEFAULT_CMP0077 NEW )
set ( LOG4CPLUS_BUILD_TESTING OFF )
set ( WITH_UNIT_TESTS OFF )
add_subdirectory ( log4cplus )
