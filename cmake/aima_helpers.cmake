if ( DEFINED AIMA_HELPERS_CMAKE_FILE_INCLUDED )
    return ()
endif ()
set ( AIMA_HELPERS_CMAKE_FILE_INCLUDED )

function ( symlink_assets )
    execute_process (
            COMMAND ${CMAKE_COMMAND} -E create_symlink "${PROJECT_SOURCE_DIR}/assets" "${CMAKE_CURRENT_BINARY_DIR}/assets"
    )
endfunction ()

unset ( AIMA_DEMOS CACHE )
unset ( AIMA_APPS CACHE )

function ( add_integratable_executable name )
    set ( options DEMO APP )
    set ( oneValueArgs CLASS )
    set ( multiValueArgs "" )
    cmake_parse_arguments ( local "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    if ( NOT DEFINED local_CLASS )
        message ( FATAL_ERROR "integratable ${name} does not have a class set" )
    elseif ( local_DEMO AND local_APP )
        message ( FATAL_ERROR "integratable ${name} was set as both a demo and an app" )
    elseif ( NOT local_DEMO AND NOT local_APP )
        message ( FATAL_ERROR "integratable ${name} was not set as a demo or an app" )
    elseif ( local_DEMO )
        list ( APPEND AIMA_DEMOS "${name}" )
        set ( AIMA_DEMOS ${AIMA_DEMOS} CACHE INTERNAL "AIMA_DEMOS" )
        set ( TYPE "DEMO" )
        set ( FILE "${PROJECT_SOURCE_DIR}/src/demos/${local_CLASS}" )
        set ( REGISTRATION ${PROJECT_SOURCE_DIR}/src/core/registerDemo.hpp )
    else ()
        list ( APPEND AIMA_APPS "${name}" )
        set ( AIMA_APPS ${AIMA_APPS} CACHE INTERNAL "AIMA_APPS" )
        set ( TYPE "APP" )
        set ( FILE "${PROJECT_SOURCE_DIR}/src/apps/${local_CLASS}" )
        set ( REGISTRATION ${PROJECT_SOURCE_DIR}/src/core/registerApp.hpp )
    endif ()

    add_library ( ${name} "${FILE}.hpp" "${FILE}.cpp" "${REGISTRATION}" )
    target_include_directories ( ${name} PUBLIC "${CMAKE_HOME_DIRECTORY}/src" )

    set ( NOTICE "THIS FILE IS AUTO-GENERATED!! DO NOT EDIT!!" )

    set ( MACRO AIMA_STANDALONE_APP )
    set ( in_file "${CMAKE_HOME_DIRECTORY}/cmake/aima.cpp.in" )
    set ( out_file "${CMAKE_BINARY_DIR}/aima/standalones/${name}.cpp" )
    configure_file ( ${in_file} ${out_file} @ONLY )

    add_executable ( ${name}_exe ${out_file} )
    target_link_libraries ( ${name}_exe PRIVATE ${name} Boost::boost )

    set ( MACRO AIMA_INTEGRATED_APP )
    set ( in_file "${CMAKE_HOME_DIRECTORY}/cmake/aima.cpp.in" )
    set ( out_file "${CMAKE_BINARY_DIR}/aima/registrations/${name}.cpp" )
    configure_file ( ${in_file} ${out_file} @ONLY )

    symlink_assets ()
endfunction ()

function ( link_demos target )
    message ( STATUS "linking demos to ${target}: ${AIMA_DEMOS}" )
    target_link_libraries ( ${target} PRIVATE ${AIMA_DEMOS} )
    foreach ( demo IN LISTS AIMA_DEMOS )
        target_sources ( ${target} PRIVATE "${CMAKE_BINARY_DIR}/aima/registrations/${demo}.cpp" )
    endforeach ()
endfunction ()

function ( link_apps target )
    message ( STATUS "linking apps to ${target}: ${AIMA_APPS}" )
    target_link_libraries ( ${target} PRIVATE ${AIMA_APPS} )
    foreach ( app IN LISTS AIMA_APPS )
        target_sources ( ${target} PRIVATE "${CMAKE_BINARY_DIR}/aima/registrations/${app}.cpp" )
    endforeach ()

    symlink_assets ()
endfunction ()
