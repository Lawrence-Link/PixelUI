cmake_minimum_required(VERSION 3.16)
project(U8g2Emulator LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# -------------------------------
# U8G2 Path
# -------------------------------
set(U8G2_C_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/csrc")
set(U8G2_CPP_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/cppsrc")

file(GLOB U8G2_C_SOURCES "${U8G2_C_SRC_DIR}/*.c")

add_library(u8g2_c STATIC ${U8G2_C_SOURCES})

if (APPLE)
target_compile_definitions(u8g2_c PRIVATE U8G2_USE_LARGE_FONTS) # in order to use the wqy fonts
endif()

target_include_directories(u8g2_c PUBLIC ${U8G2_C_SRC_DIR})

file(GLOB U8G2_CPP_SOURCES "${U8G2_CPP_SRC_DIR}/*.cpp")
file(GLOB U8G2_HEADERS "${U8G2_C_SRC_DIR}/*.h" "${U8G2_CPP_SRC_DIR}/*.h")

# Debug 选项
option(ENABLE_DEBUG "Enable debug mode" ON)

if(ENABLE_DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
    add_definitions(-DDEBUG)
endif()

# -------------------------------
# Simulator source code
# -------------------------------
set(SIM_SOURCES
    ${U8G2_CPP_SOURCES}
    main.cpp
    u8g2_wrapper.cpp
    MainWindow.cpp
    EmuWorker.cpp
    ../examples/app_info.cpp
    ../examples/app_dynamic_info.cpp
    ../examples/app_cube_demo.cpp
    ../examples/charging_animation.cpp
    ../examples/app_counter.cpp
    ../examples/app_pixel_clock.cpp
    ../examples/ListViewDemo1.cpp
    ../examples/app_widgets_demo/app_demo_analogclock_numscroll.cpp
    ../examples/app_widgets_demo/app_demo_textbutton_label.cpp
    ../examples/app_widgets_demo/app_demo_animation.cpp
    ../examples/app_greenhouse/app_main.cpp
)

    list(APPEND SIM_SOURCES 
        ../examples/app_registry.cpp
    )

add_executable(u8g2_emulator ${SIM_SOURCES})

# -------------------------------
# 包含目录（ETL + PixelUI + U8G2）
# -------------------------------
target_include_directories(u8g2_emulator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${U8G2_CPP_SRC_DIR}
        ${U8G2_C_SRC_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/etl/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../examples/
)

# -------------------------------
# 链接库
# -------------------------------
target_link_libraries(u8g2_emulator
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        pixelui
        u8g2_c
)

# -------------------------------
# 编译定义
# -------------------------------
set_target_properties(u8g2_emulator PROPERTIES
    COMPILE_DEFINITIONS "ASAN_OPTIONS=\"detect_leaks=1:abort_on_error=1\""
)

target_compile_definitions(u8g2_emulator PRIVATE USE_QT_GUI USE_DEBUG_OUPUT USE_EMULATOR)
