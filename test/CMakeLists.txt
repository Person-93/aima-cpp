if ( NOT AIMA_ENABLE_TESTING )
    return ()
endif ()

include ( CTest )

include_directories ( ../src )

add_library ( test_base STATIC main.cpp )
target_link_libraries ( test_base PUBLIC logging Boost::unit_test_framework )

add_executable ( test_asset_manager src/util/AssetManager.cpp )
target_link_libraries ( test_asset_manager asset_manager test_base )
add_test ( NAME test_asset_manager COMMAND test_asset_manager )

add_executable ( test_geometry src/PathFinder/geometry.cpp )
target_link_libraries ( test_geometry path_finder_base test_base )
add_test ( NAME test_geometry COMMAND test_geometry )
