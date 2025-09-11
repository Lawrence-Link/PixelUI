cmake_minimum_required(VERSION 3.16)
project(U8g2Emulator LANGUAGES CXX C)

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

set(GIT_DIR_LOOKUP_POLICY ALLOW_LOOKING_ABOVE_CMAKE_SOURCE_DIR)
add_subdirectory(third_party/etl)

set(U8G2_C_SRC_DIR "third_party/u8g2/csrc")
set(U8G2_CPP_SRC_DIR "third_party/u8g2/cppsrc")

# --- u8g2 core C library ---
file(GLOB U8G2_C_SOURCES "${U8G2_C_SRC_DIR}/*.c")
add_library(u8g2_c STATIC ${U8G2_C_SOURCES})
target_include_directories(u8g2_c PUBLIC ${U8G2_C_SRC_DIR})

# --- u8g2 C++ wrapper sources ---
file(GLOB U8G2_CPP_SOURCES "${U8G2_CPP_SRC_DIR}/*.cpp")
file(GLOB U8G2_HEADERS "${U8G2_C_SRC_DIR}/*.h" "${U8G2_CPP_SRC_DIR}/*.h")

set(SOURCES
    ${U8G2_CPP_SOURCES}
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
    src/PixelUI/core/animation/animation.cpp
    src/PixelUI/core/ui/AppView/AppView.cpp
    src/PixelUI/core/ui/Popup/Popup.cpp
    src/PixelUI/core/ui/Popup/PopupManager.cpp
    src/PixelUI/core/ViewManager/ViewManager.cpp
    src/PixelUI/core/ui/ListView/ListView.cpp
    src/ListViewDemo/ListViewDemo1.cpp
    src/PixelUI/core/widgets/histogram/histogram.cpp
    src/PixelUI/core/widgets/brace/brace.cpp
    src/app_counter.cpp
)

set(HEADERS
    ${U8G2_HEADERS}
    include/u8g2_wrapper.h
    include/MainWindow.h
    include/EmuWorker.h
    include/PixelUI/pixelui.h
    include/PixelUI/core/menu/menu_system.h

    include/PixelUI/core/animation/animation.h
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
    include/PixelUI/core/widgets/histogram/histogram.h
    include/PixelUI/core/widgets/brace/brace.h
)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(u8g2_emulator ${SOURCES} ${HEADERS})

target_link_libraries(u8g2_emulator
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    etl::etl
    u8g2_c         # <- 链接上C库
)

target_compile_definitions(u8g2_emulator PRIVATE USE_QT_GUI)

target_include_directories(u8g2_emulator
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${U8G2_CPP_SRC_DIR}
    SYSTEM PRIVATE
)
