cmake_minimum_required(VERSION 3.16)
project(PixelUI LANGUAGES CXX)

set(U8G2_C_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/csrc")
set(U8G2_CPP_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/cppsrc")

# use static app register
option(USE_STATIC_APP_REGISTER "Use static app register" ON) 

add_library(pixelui STATIC
    PixelUI.cpp
    core/app/app_system.cpp
    core/animation/animation.cpp
    core/ViewManager/ViewManager.cpp
    core/coroutine/Coroutine.cpp
    
    ui/Popup/Popup.cpp
    ui/ListView/ListView.cpp
    ui/AppLauncher/AppLauncher.cpp
    ui/IconView/IconView.cpp
    
    widgets/histogram/histogram.cpp
    widgets/brace/brace.cpp
    widgets/iconButton/iconButton.cpp
    focus/focus.cpp
    blinker/Blinker.cpp
)

if (USE_STATIC_APP_REGISTER)
    target_compile_definitions(pixelui 
    PUBLIC 
    USE_STATIC_APP_REGISTER_ENABLED=1)
endif()

target_include_directories(pixelui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/etl/include
        ${U8G2_CPP_SRC_DIR}
        ${U8G2_C_SRC_DIR}
)
