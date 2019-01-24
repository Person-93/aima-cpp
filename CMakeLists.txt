cmake_minimum_required ( VERSION 3.13 )
list ( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake )

project ( aima_cpp VERSION "0.1.0" )
set ( CMAKE_CXX_STANDARD 17 )

include_directories ( src )

add_subdirectory ( src )
