cmake_minimum_required(VERSION 3.16)
project(PixelUI LANGUAGES CXX)

set(U8G2_C_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/csrc")
set(U8G2_CPP_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/cppsrc")

add_library(pixelui
    PixelUI.cpp
    core/app/app_system.cpp
    core/animation/animation.cpp
    ui/AppView/AppView.cpp
    ui/Popup/Popup.cpp
    ui/ListView/ListView.cpp
    core/ViewManager/ViewManager.cpp
    widgets/histogram/histogram.cpp
    widgets/brace/brace.cpp
    widgets/iconButton/iconButton.cpp
    focus/focus.cpp
)

target_include_directories(pixelui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/etl/include
        ${U8G2_CPP_SRC_DIR}
        ${U8G2_C_SRC_DIR}
)
