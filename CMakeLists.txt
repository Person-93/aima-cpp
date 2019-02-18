cmake_minimum_required ( VERSION 3.13 )

if ( DEFINED CMAKE_TOOLCHAIN_FILE )
    message ( STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}" )
endif ()

project ( aima_cpp VERSION "0.1.0" )
set ( CMAKE_CXX_STANDARD 17 )

set ( Boost_USE_MULTITHREADED ON )
set ( OpenGL_GL_PREFERENCE GLVND )

find_package ( Threads REQUIRED )
find_package ( Boost REQUIRED )
find_package ( OpenGL REQUIRED )
find_package ( glfw3 REQUIRED )

target_link_libraries ( glfw INTERFACE GL )

add_subdirectory ( vendor )
add_subdirectory ( src )
