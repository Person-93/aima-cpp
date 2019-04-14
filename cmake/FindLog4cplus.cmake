if ( NOT Log4cplus_FOUND )
    find_path ( Log4cplus_INCLUDE_DIR NAMES log4cplus/config.hxx log4cplus/appender.h log4cplus/loglevel.h )
    find_library ( Log4cplus_LIBRARY log4cplus )

    include ( FindPackageHandleStandardArgs )
    find_package_handle_standard_args ( Log4cplus REQUIRED_VARS Log4cplus_LIBRARY Log4cplus_INCLUDE_DIR )

    if ( Log4cplus_FOUND )
        add_library ( Log4cplus INTERFACE )
        target_include_directories ( Log4cplus INTERFACE ${Log4cplus_INCLUDE_DIR} )
        target_link_libraries ( Log4cplus INTERFACE ${Log4cplus_LIBRARY} )
    endif ()
endif ()

mark_as_advanced ( Log4cplus_INCLUDE_DIR SQLITE3_LIBRARIES )
