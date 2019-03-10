include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} )

add_library ( core STATIC
              core/Environment.cpp
              core/Action.cpp
              )
target_link_libraries ( core PUBLIC atomic ${CMAKE_THREAD_LIBS_INIT} )

add_library ( viewer_simple STATIC viewers/SimpleViewer.cpp )

add_library ( app STATIC
              viewers/GraphicViewer.cpp
              gui/ImGuiWrapper.cpp
              gui/OutputConsoleWidget.cpp
              )
target_link_libraries ( app PUBLIC imgui viewer_simple Boost::boost )

add_library ( vacuum STATIC
              vacuum/BasicVacuumEnvironment.cpp
              vacuum/BasicVacuumAgent.cpp
              vacuum/StatefulVacuumAgent.cpp
              vacuum/LocalVacuumEnvironmentPercept.cpp
              vacuum/operators.cpp
              )
target_link_libraries ( vacuum PRIVATE core )

add_library ( vacuum_demo INTERFACE )
target_link_libraries ( vacuum_demo INTERFACE vacuum viewer_simple )

add_executable ( trivial_vacuum_demo vacuum/trivial_vacuum_demo.cpp )
target_link_libraries ( trivial_vacuum_demo PUBLIC vacuum_demo )
target_compile_definitions ( trivial_vacuum_demo INTERFACE AIMA_INTEGRATED_APP )

add_executable ( stateful_vacuum_demo vacuum/stateful_vacuum_demo.cpp )
target_link_libraries ( stateful_vacuum_demo PUBLIC vacuum_demo )
target_compile_definitions ( stateful_vacuum_demo INTERFACE AIMA_INTEGRATED_APP )

add_library ( vacuum_app STATIC vacuum/VacuumGui.cpp )
target_link_libraries ( vacuum_app PUBLIC vacuum app )

add_executable ( trivial_vacuum_app vacuum/TrivialVacuumApp.cpp )
target_link_libraries ( trivial_vacuum_app PUBLIC vacuum_app )
target_compile_definitions ( trivial_vacuum_app INTERFACE AIMA_INTEGRATED_APP )
