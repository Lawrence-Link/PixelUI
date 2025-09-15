cmake_minimum_required(VERSION 3.16)
project(U8g2Emulator LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# -------------------------------
# U8G2 路径
# -------------------------------
set(U8G2_C_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/csrc")
set(U8G2_CPP_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/cppsrc")

file(GLOB U8G2_C_SOURCES "${U8G2_C_SRC_DIR}/*.c")
add_library(u8g2_c STATIC ${U8G2_C_SOURCES})
target_include_directories(u8g2_c PUBLIC ${U8G2_C_SRC_DIR})

file(GLOB U8G2_CPP_SOURCES "${U8G2_CPP_SRC_DIR}/*.cpp")
file(GLOB U8G2_HEADERS "${U8G2_C_SRC_DIR}/*.h" "${U8G2_CPP_SRC_DIR}/*.h")

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
    ../examples/ListViewDemo1.cpp

    # PixelUI Source code
    ../src/PixelUI.cpp
    ../src/core/app/app_system.cpp
    ../src/core/animation/animation.cpp
    ../src/ui/AppView/AppView.cpp
    ../src/ui/Popup/Popup.cpp
    ../src/ui/ListView/ListView.cpp
    ../src/core/ViewManager/ViewManager.cpp
    ../src/widgets/histogram/histogram.cpp
    ../src/widgets/brace/brace.cpp
)

set(SIM_HEADERS
    ${U8G2_HEADERS}
    u8g2_wrapper.h
    MainWindow.h
    EmuWorker.h
    ../include/PixelUI.h
    ../include/core/animation/animation.h
    ../include/core/app/app_system.h
    ../include/core/app/IApplication.h
    ../include/ui/AppView/AppView.h
    ../include/ui/IDrawable.h
    ../include/ui/Popup/Popup.h
    ../include/core/ViewManager/ViewManager.h
    ../include/config.h
    ../include/ui/ListView/ListView.h
    ../include/core/CommonTypes.h
    ../include/widgets/histogram/histogram.h
    ../include/widgets/brace/brace.h
)

add_executable(u8g2_emulator ${SIM_SOURCES} ${SIM_HEADERS})

# -------------------------------
# 包含目录（ETL + PixelUI + U8G2）
# -------------------------------
target_include_directories(u8g2_emulator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${U8G2_CPP_SRC_DIR}
        ${U8G2_C_SRC_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/etl/include
)

# -------------------------------
# 链接库
# -------------------------------
target_link_libraries(u8g2_emulator
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        u8g2_c
        pixelui
)

# -------------------------------
# 编译定义
# -------------------------------
target_compile_definitions(u8g2_emulator PRIVATE USE_QT_GUI USE_DEBUG_OUPUT USE_EMULATOR)
