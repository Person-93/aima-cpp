add_library ( vacuum_base
              BasicVacuumEnvironment.cpp
              BasicVacuumEnvironment.hpp
              BasicVacuumAgent.cpp
              BasicVacuumAgent.hpp
              StatefulVacuumAgent.cpp
              StatefulVacuumAgent.hpp
              LocalVacuumEnvironmentPercept.cpp
              LocalVacuumEnvironmentPercept.hpp
              GlobalVacuumEnvironmentPercept.cpp
              GlobalVacuumEnvironmentPercept.hpp
              SearchBasedAgent.cpp
              SearchBasedAgent.hpp
              VisibleVacuumEnvironment.cpp
              VisibleVacuumEnvironment.hpp
              misc.cpp
              misc.hpp
              )
target_link_libraries ( vacuum_base PUBLIC common )

add_library ( vacuum_demo_base INTERFACE )
target_link_libraries ( vacuum_demo_base INTERFACE vacuum_base demo_base )

add_library ( vacuum_app_base
              VacuumGui.hpp
              VacuumGui.cpp
              )
target_link_libraries ( vacuum_app_base PUBLIC app_base vacuum_base )

add_integratable_executable ( trivial_vacuum_demo DEMO CLASS TrivialVacuumDemo )
target_link_libraries ( trivial_vacuum_demo PUBLIC vacuum_demo_base )

add_integratable_executable ( stateful_vacuum_demo DEMO CLASS StatefulVacuumDemo )
target_link_libraries ( stateful_vacuum_demo PUBLIC vacuum_demo_base )

add_integratable_executable ( trivial_vacuum_app APP CLASS TrivialVacuumApp )
target_link_libraries ( trivial_vacuum_app PUBLIC vacuum_app_base )

add_integratable_executable ( stateful_vacuum_app APP CLASS StatefulVacuumApp )
target_link_libraries ( stateful_vacuum_app PUBLIC vacuum_app_base )

add_integratable_executable ( reflex_vacuum_app APP CLASS RandomnessVacuumApp )
target_sources ( reflex_vacuum_app PUBLIC
                 RandomnessBasedAgent.cpp
                 RandomnessBasedAgent.hpp
                 )
target_link_libraries ( reflex_vacuum_app PUBLIC vacuum_app_base )

add_integratable_executable ( search_vacuum_app APP CLASS SearchVacuumApp )
target_sources ( search_vacuum_app PUBLIC
                 IterativeLengtheningAgent.cpp
                 IterativeLengtheningAgent.hpp
                 )
target_link_libraries ( search_vacuum_app PUBLIC vacuum_app_base )
