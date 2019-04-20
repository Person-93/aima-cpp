if ( NOT AIMA_ENABLE_TESTING )
    return ()
endif ()

include ( CTest )

include_directories ( ../src )

add_executable ( test_asset_manager
                 main.cpp
                 src/util/AssetManager.cpp
                 )
target_link_libraries ( test_asset_manager PRIVATE asset_manager Boost::unit_test_framework )
add_test ( NAME test_asset_manager COMMAND test_asset_manager )