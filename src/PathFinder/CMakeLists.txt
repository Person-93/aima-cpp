add_library ( path_finder_base
              Point.cpp
              Point.hpp
              LineSegment.cpp
              LineSegment.hpp
              Polygon.cpp
              Polygon.hpp
              geometry.hpp
              geometry.cpp
              PathFinderEnvironment.cpp
              PathFinderEnvironment.hpp
              PathFinderPercept.cpp
              PathFinderPercept.hpp
              PathFinderAgent.cpp
              PathFinderAgent.hpp
              SearchNode.hpp
              IterativeDeepeningAgent.cpp
              IterativeDeepeningAgent.hpp
              AStarAgent.cpp
              AStarAgent.hpp
              )
target_link_libraries ( path_finder_base PUBLIC common cppcoro )

add_library ( path_finder_demo_base INTERFACE )
target_link_libraries ( path_finder_demo_base INTERFACE path_finder_base demo_base )

add_library ( path_finder_app_base
              PathFinderGui.cpp
              PathFinderGui.hpp
              )
target_link_libraries ( path_finder_app_base PUBLIC path_finder_base app_base )

add_integratable_executable ( path_finder_demo DEMO CLASS PathFinderDemo )
target_link_libraries ( path_finder_demo PUBLIC path_finder_demo_base )

add_integratable_executable ( path_finder_app APP CLASS PathFinderApp )
target_link_libraries ( path_finder_app PUBLIC path_finder_app_base )
