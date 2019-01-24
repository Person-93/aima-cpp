add_library ( core STATIC
              core/Environment.cpp
              core/Action.cpp
              )

add_library ( viewer_simple STATIC viewers/SimpleViewer.cpp )

add_library ( vacuum STATIC
              vacuum/BasicVacuumEnvironment.cpp
              vacuum/BasicVacuumAgent.cpp
              vacuum/LocalVacuumEnvironmentPercept.cpp
              vacuum/operators.cpp
              )
target_link_libraries ( vacuum PRIVATE core )

add_library ( vacuum_demo INTERFACE )
target_link_libraries ( vacuum_demo INTERFACE vacuum viewer_simple )

add_executable ( trivial_vacuum_demo vacuum/trivial_vacuum_demo.cpp )
target_link_libraries ( trivial_vacuum_demo PRIVATE vacuum_demo )

add_executable ( stateful_vacuum_demo vacuum/stateful_vacuum_demo.cpp )
target_link_libraries ( stateful_vacuum_demo PRIVATE vacuum_demo )
