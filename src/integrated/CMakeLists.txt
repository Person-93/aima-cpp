add_library ( integrated_app
              IntegratedRunner.cpp
              IntegratedRunner.hpp
              IntegratedDemo.cpp
              IntegratedDemo.hpp
              DemoRegistry.cpp
              DemoRegistry.hpp
              DemoRegistryEntry.cpp
              DemoRegistryEntry.hpp
              AppRegistryEntry.cpp
              AppRegistryEntry.hpp
              AppRegistry.cpp
              IntegratedDemo.hpp
              forward.hpp
              )
target_link_libraries ( integrated_app PUBLIC app_base demo_base )
link_demos ( integrated_app )
link_apps ( integrated_app )

add_executable ( integrated_app_exe main.cpp )
target_link_libraries ( integrated_app_exe PRIVATE Boost::boost integrated_app )
