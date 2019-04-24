add_library ( integrated_app
              integrated/IntegratedRunner.cpp
              integrated/IntegratedRunner.hpp
              integrated/IntegratedDemo.cpp
              integrated/IntegratedDemo.hpp
              integrated/DemoRegistry.cpp
              integrated/DemoRegistry.hpp
              integrated/AppRegistry.cpp
              integrated/IntegratedDemo.hpp
              integrated/forward.hpp
              )
target_link_libraries ( integrated_app PUBLIC app_base demo_base )
link_demos ( integrated_app )
link_apps ( integrated_app )

add_executable ( integrated_app_exe integrated/main.cpp )
target_link_libraries ( integrated_app_exe PRIVATE Boost::boost integrated_app )
