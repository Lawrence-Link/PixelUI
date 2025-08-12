cmake_minimum_required(VERSION 3.16)
project(U8g2Emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 必须放在前面，这样 Qt 自动 MOC/UIC/RCC 才生效
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

option(WITH_QT "Enable Qt GUI mode" OFF)

# ==============================
# 1. u8g2 submodule
# ==============================
add_subdirectory(third_party/u8g2 EXCLUDE_FROM_ALL)

# ==============================
# 2. Source files
# ==============================
set(SOURCES
    src/main.cpp
    src/u8g2_wrapper.cpp
    src/MainWindow.cpp
    src/EmuWorker.cpp
    src/PixelScriptUI/pixelscriptui.cpp
    src/PixelScriptUI/menu_manager.cpp
    src/PixelScriptUI/root_menu.cpp
    src/PixelScriptUI/app_1.cpp
)

set(HEADERS
    include/u8g2_wrapper.h
    include/MainWindow.h
    include/EmuWorker.h
    include/PixelScriptUI/pixelscriptui.h
    include/PixelScriptUI/menu_manager.h
    include/PixelScriptUI/root_menu.h
)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(u8g2_emulator ${SOURCES} ${HEADERS})

target_link_libraries(u8g2_emulator
    PRIVATE
    u8g2
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_compile_definitions(u8g2_emulator PRIVATE USE_QT_GUI)

# ==============================
# 3. Headers
# ==============================
target_include_directories(u8g2_emulator PRIVATE
    include
    third_party/u8g2/csrc
)
