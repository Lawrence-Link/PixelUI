cmake_minimum_required(VERSION 3.16)
project(PixelUI_Project LANGUAGES CXX C)

include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build simulator
option(BUILD_SIMULATOR "Build PC Qt simulator" ON)
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

if(ENABLE_ASAN AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer -g -O1)
    add_link_options(-fsanitize=address)
    message(STATUS "AddressSanitizer enabled")
endif()

if(NOT BUILD_SIMULATOR)
    add_subdirectory(third_party/etl EXCLUDE_FROM_ALL)
    add_subdirectory(third_party/u8g2 EXCLUDE_FROM_ALL)
endif()

add_subdirectory(src)

if(NOT BUILD_SIMULATOR)
    if(NOT TARGET u8g2)
        message(FATAL_ERROR "u8g2 target not found! Please provide your cropped u8g2 library.")
    endif()
    if(NOT TARGET etl::etl)
        message(FATAL_ERROR "etl::etl target not found! Please provide your cropped ETL library.")
    endif()
endif()

# -------------------------------
# Simulator build
# -------------------------------
if(BUILD_SIMULATOR)
    message(STATUS "Building PixelUI simulator (PC Qt)")
    add_subdirectory(simulator)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()
endif()

target_compile_options(pixelui PUBLIC -fno-exceptions -fno-rtti)
