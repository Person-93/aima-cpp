add_library ( logging
              util/configure_logging.cpp
              util/configure_logging.hpp
              )
target_link_libraries ( logging PUBLIC Log4cplus )
target_compile_definitions ( logging PUBLIC APPLICATION_LOG_LEVEL=${LOG_LEVEL}_LOG_LEVEL )

add_library ( version version.hpp ${version_file} )

add_library ( common
              core/Environment.cpp
              core/Environment.hpp
              core/Action.cpp
              core/Action.hpp
              views/SimpleViewer.cpp
              views/SimpleViewer.hpp
              util/ThreadSafeWrapper.cpp
              util/ThreadSafeWrapper.hpp
              util/random_string.cpp
              util/random_string.hpp
              util/Container.hpp
              util/parseTitle.hpp
              util/StringBuilder.hpp
              util/type_traits.hpp
              util/UniqueIdMixin.hpp
              )
target_link_libraries ( common PUBLIC version logging atomic )

add_library ( imgui
              gui/ImGuiWrapper.hpp
              gui/ImGuiWrapper.cpp
              )
target_link_libraries ( imgui PUBLIC imgui_base common )
target_compile_definitions ( imgui PUBLIC IMGUI_USER_CONFIG="${PROJECT_SOURCE_DIR}/src/gui/imgui_config.hpp" )

add_library ( asset_manager
              util/AssetManager.hpp
              util/AssetManager.cpp
              util/asset_manager/TypeErasedContainer.hpp
              util/asset_manager/TypeErasedContainer.cpp
              util/asset_manager/AssetManagerException.hpp
              )
target_link_libraries ( asset_manager PRIVATE SQLite )
target_link_libraries ( asset_manager PUBLIC common stdc++fs )

add_library ( demo_base
              core/Demo.cpp
              core/Demo.hpp
              core/DemoRegistryEntry.cpp
              core/DemoRegistryEntry.hpp
              )
target_link_libraries ( demo_base PUBLIC common )

add_library ( app_base
              core/App.cpp
              core/App.hpp
              core/AppPtr.cpp
              core/AppPtr.hpp
              core/AppRegistryEntry.cpp
              core/AppRegistryEntry.hpp
              views/GraphicViewer.cpp
              views/GraphicViewer.hpp
              gui/OutputConsoleWidget.cpp
              gui/OutputConsoleWidget.hpp
              )
target_link_libraries ( app_base PUBLIC imgui common Threads::Threads )
target_link_libraries ( app_base INTERFACE asset_manager )
