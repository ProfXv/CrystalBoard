cmake_minimum_required(VERSION 3.5)

project(CrystalBoard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Place the executable in the project root directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(CrystalBoard
    src/main.cpp
    src/mainwindow.cpp
    src/canvas.cpp
    src/helppanel.cpp
)

# Ensure macOS creates a .app bundle for GUI applications
if(APPLE)
    set_target_properties(CrystalBoard PROPERTIES
        MACOSX_BUNDLE TRUE
    )
endif()

# --- Qt ---
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)
qt_standard_project_setup()

target_link_libraries(CrystalBoard PRIVATE Qt6::Widgets)
