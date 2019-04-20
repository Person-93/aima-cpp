cmake_minimum_required ( VERSION 3.13 )
if ( DEFINED CMAKE_TOOLCHAIN_FILE )
    message ( STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}" )
endif ()
list ( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake )

project ( aima_cpp VERSION "0.1.0" )

set ( CMAKE_CXX_STANDARD 17 )
option ( AIMA_ENABLE_TESTING "Build the test cases" OFF )
set ( LOG_LEVEL "DEBUG" CACHE STRING "Log level to use for Log4cplus" )
set ( BUILD_SHARED_LIBS CACHE BOOL ON "Build shared libraries" )
set ( CMAKE_LINK_DEPENDS_NO_SHARED ON )
set ( Boost_USE_MULTITHREADED ON )
set ( OpenGL_GL_PREFERENCE GLVND )

if ( AIMA_ENABLE_TESTING )
    set ( BOOST_MODULES unit_test_framework )
endif ()

find_package ( Log4cplus REQUIRED )
find_package ( Threads REQUIRED )
find_package ( Boost REQUIRED ${BOOST_MODULES} )
find_package ( OpenGL REQUIRED )
find_package ( glfw3 REQUIRED )
find_package ( SQLite3 REQUIRED )

target_link_libraries ( glfw INTERFACE GL )

add_subdirectory ( vendor )
add_subdirectory ( src )
add_subdirectory ( test )
