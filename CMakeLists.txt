cmake_minimum_required(VERSION 3.21)
project(dashboard VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

include(GNUInstallDirs)

if(NOT TARGET widget-sdk)
    find_package(widget-sdk REQUIRED)
endif()

set(SOURCES
    src/main.cpp
    src/app/DashboardApp.cpp
    src/core/PluginLoader.cpp
    src/core/WidgetManager.cpp
    src/core/LayoutEngine.cpp
    src/core/ConfigStore.cpp
    src/core/WidgetDataStore.cpp
    src/ui/DashboardWindow.cpp
    src/ui/WidgetCanvas.cpp
    src/ui/WidgetFrame.cpp
    src/ui/SettingsDialog.cpp
    src/ui/AddWidgetDialog.cpp
    src/ui/TitleBar.cpp
)

set(HEADERS
    src/app/DashboardApp.h
    src/core/PluginLoader.h
    src/core/WidgetManager.h
    src/core/LayoutEngine.h
    src/core/ConfigStore.h
    src/core/WidgetDataStore.h
    src/ui/DashboardWindow.h
    src/ui/WidgetCanvas.h
    src/ui/WidgetFrame.h
    src/ui/SettingsDialog.h
    src/ui/AddWidgetDialog.h
    src/ui/TitleBar.h
)

add_executable(dashboard ${SOURCES} ${HEADERS} resources/dashboard.qrc)

target_include_directories(dashboard PRIVATE src)
target_link_libraries(dashboard PRIVATE Qt6::Widgets widget-sdk)

# Place plugins next to executable for easy discovery
set_target_properties(dashboard PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dashboard
)

# Installed plugin path baked in at compile time (requires -DCMAKE_INSTALL_PREFIX=/usr)
target_compile_definitions(dashboard PRIVATE
    DASHBOARD_PLUGIN_DIR="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/dashboard/plugins"
)

# No RPATH needed — Qt ships to standard /usr/lib paths on target systems
set_target_properties(dashboard PROPERTIES
    INSTALL_RPATH ""
    BUILD_WITH_INSTALL_RPATH FALSE
)

install(TARGETS dashboard
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(FILES resources/dashboard.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)
install(FILES resources/dashboard.svg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
)

# Optional: bundle pre-built widget plugins into the dashboard install tree.
# Point this at the directory that holds the widget *.so and *.json files.
# Example:
#   cmake -S dashboard -B build/dashboard \
#         -DCMAKE_PREFIX_PATH=~/.local \
#         -DWIDGET_PLUGIN_DIRS=/path/to/dist/lib/dashboard/plugins
if(DEFINED WIDGET_PLUGIN_DIRS)
    install(DIRECTORY "${WIDGET_PLUGIN_DIRS}/"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/dashboard/plugins
        FILES_MATCHING
            PATTERN "*.so"
            PATTERN "*.json"
    )
endif()
