add_library ( vacuum_base
              vacuum/BasicVacuumEnvironment.cpp
              vacuum/BasicVacuumEnvironment.hpp
              vacuum/BasicVacuumAgent.cpp
              vacuum/BasicVacuumAgent.hpp
              vacuum/StatefulVacuumAgent.cpp
              vacuum/StatefulVacuumAgent.hpp
              vacuum/LocalVacuumEnvironmentPercept.cpp
              vacuum/LocalVacuumEnvironmentPercept.hpp
              vacuum/misc.cpp
              vacuum/misc.hpp
              )
target_link_libraries ( vacuum_base PUBLIC common )

add_library ( vacuum_demo_base INTERFACE )
target_link_libraries ( vacuum_demo_base INTERFACE vacuum_base demo_base )

add_library ( vacuum_interface
              vacuum/VacuumGui.hpp
              vacuum/VacuumGui.cpp
              )
target_link_libraries ( vacuum_interface PUBLIC user_interface vacuum_base )

add_library ( vacuum_app_base INTERFACE )
target_link_libraries ( vacuum_app_base INTERFACE app_base vacuum_interface )


add_integratable_executable ( trivial_vacuum_demo DEMO CLASS TrivialVacuumDemo )
target_link_libraries ( trivial_vacuum_demo PUBLIC vacuum_demo_base )

add_integratable_executable ( stateful_vacuum_demo DEMO CLASS StatefulVacuumDemo )
target_link_libraries ( stateful_vacuum_demo PUBLIC vacuum_demo_base )

add_integratable_executable ( trivial_vacuum_app APP CLASS TrivialVacuumApp )
target_link_libraries ( trivial_vacuum_app PUBLIC vacuum_app_base )

add_integratable_executable ( stateful_vacuum_app APP CLASS StatefulVacuumApp )
target_link_libraries ( stateful_vacuum_app PUBLIC vacuum_app_base )
