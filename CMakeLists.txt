cmake_minimum_required ( VERSION 3.13 )
if ( DEFINED CMAKE_TOOLCHAIN_FILE )
    message ( STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}" )
endif ()
list ( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake )

project ( aima_cpp )
include ( GetGitRevisionDescription )

git_describe ( VERSION --tags --dirty=-dirty )
message ( STATUS "Configuring ${PROJECT_NAME} ${VERSION}" )
set_version_vars ( "${VERSION}" "VERSION" )

set ( version_file ${CMAKE_CURRENT_BINARY_DIR}/version.cpp )
configure_file ( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.cpp.in" "${version_file}" @ONLY )

set ( CMAKE_CXX_STANDARD 17 )
option ( AIMA_ENABLE_TESTING "Build the test cases" OFF )
set ( LOG_LEVEL "DEBUG" CACHE STRING "Log level to use for Log4cplus" )
if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    option ( BUILD_SHARED_LIBS "Build shared libraries" ON )
else ()
    option ( BUILD_SHARED_LIBS "Build shared libraries" OFF )
endif ()
message ( STATUS "Building shared libs: ${BUILD_SHARED_LIBS}" )
set ( CMAKE_LINK_DEPENDS_NO_SHARED ON )
set ( Boost_USE_MULTITHREADED ON )
set ( OpenGL_GL_PREFERENCE GLVND )

if ( AIMA_ENABLE_TESTING )
    set ( BOOST_MODULES unit_test_framework )
endif ()

add_compile_options ( --stdlib=libc++ -fcoroutines-ts )
add_link_options ( --stdlib=libc++ -lc++abi )

find_package ( Threads REQUIRED )
find_package ( Boost REQUIRED ${BOOST_MODULES} )
find_package ( OpenGL REQUIRED )
find_package ( glfw3 REQUIRED )
find_package ( SQLite3 REQUIRED )

target_link_libraries ( glfw INTERFACE GL )

add_subdirectory ( vendor )
add_subdirectory ( src )
add_subdirectory ( test )
