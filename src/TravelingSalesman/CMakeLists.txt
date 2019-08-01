add_library ( traveling_salesman_base
              TravelingSalesmanEnvironment.cpp
              TravelingSalesmanEnvironment.hpp
              TravelingSalesmanPercept.cpp
              TravelingSalesmanPercept.hpp
              IterativeLengtheningAgent.cpp
              IterativeLengtheningAgent.hpp
              SearchNode.hpp
              )
target_link_libraries ( traveling_salesman_base PUBLIC common geometry )

add_library ( traveling_salesman_app_base
              TravelingSalesmanGui.cpp
              TravelingSalesmanGui.hpp
              )
target_link_libraries ( traveling_salesman_app_base PUBLIC traveling_salesman_base app_base )

add_integratable_executable ( traveling_salesman_app APP CLASS TravelingSalesmanApp )
target_link_libraries ( traveling_salesman_app PUBLIC traveling_salesman_app_base )
