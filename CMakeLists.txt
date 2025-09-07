cmake_minimum_required(VERSION 3.16)
project(U8g2Emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
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
set(GIT_DIR_LOOKUP_POLICY ALLOW_LOOKING_ABOVE_CMAKE_SOURCE_DIR)
add_subdirectory(third_party/etl)
# ==============================
# 2. Source files
# ==============================
set(SOURCES
    src/main.cpp
    src/u8g2_wrapper.cpp
    src/MainWindow.cpp
    src/EmuWorker.cpp
    src/PixelUI/pixelui.cpp
    src/PixelUI/core/menu/menu_system.cpp
    
    src/app_penis.cpp
    src/app_info.cpp
    src/app_dynamic_info.cpp
    src/app_cube_demo.cpp
    src/charging_animation.cpp

    src/PixelUI/core/app/app_system.cpp
    src/PixelUI/core/animation/anime.cpp
    src/PixelUI/core/ui/AppView/AppView.cpp
    src/PixelUI/core/ui/Popup/Popup.cpp
    src/PixelUI/core/ui/Popup/PopupManager.cpp
    src/PixelUI/core/ViewManager/ViewManager.cpp
    src/PixelUI/core/ui/ListView/ListView.cpp
    src/ListViewDemo/ListViewDemo1.cpp
)

set(HEADERS
    include/u8g2_wrapper.h
    include/MainWindow.h
    include/EmuWorker.h
    include/PixelUI/pixelui.h
    include/PixelUI/core/menu/menu_system.h

    include/app_example.h
    include/app_example2.h
    include/app_example3.h

    include/PixelUI/core/animation/anime.h
    include/PixelUI/core/app/app_system.h
    include/PixelUI/core/app/IApplication.h
    include/PixelUI/core/ui/AppView/AppView.h
    include/PixelUI/core/ui/IDrawable.h
    include/PixelUI/core/ui/Popup/IPopupRenderer.h
    include/PixelUI/core/ui/Popup/Popup.h
    include/PixelUI/core/ui/Popup/PopupManager.h
    include/PixelUI/core/ViewManager/ViewManager.h
    include/PixelUI/config.h
    include/PixelUI/core/ui/ListView/ListView.h
    include/PixelUI/core/CommonTypes.h
)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(u8g2_emulator ${SOURCES} ${HEADERS})

target_link_libraries(u8g2_emulator
    PRIVATE
    u8g2
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    etl::etl
)

target_compile_definitions(u8g2_emulator PRIVATE USE_QT_GUI)

# ==============================
# 3. Headers
# ==============================
target_include_directories(u8g2_emulator
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/third_party/u8g2/csrc
    SYSTEM PRIVATE
)