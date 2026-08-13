add_executable(application_stack_test application_stack_test.cpp)
target_link_libraries(application_stack_test PRIVATE pixelui)
target_compile_definitions(application_stack_test PRIVATE ETL_NO_STL)
add_test(NAME application_stack_test COMMAND application_stack_test)

add_executable(
    view_manager_lifecycle_test
    view_manager_lifecycle_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/cppsrc/U8g2lib.cpp
)
target_link_libraries(view_manager_lifecycle_test PRIVATE pixelui u8g2_c)
target_compile_definitions(view_manager_lifecycle_test PRIVATE ETL_NO_STL)
add_test(NAME view_manager_lifecycle_test COMMAND view_manager_lifecycle_test)

add_executable(
    chart_buffer_test
    chart_buffer_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/cppsrc/U8g2lib.cpp
)
target_link_libraries(chart_buffer_test PRIVATE pixelui u8g2_c)
target_compile_definitions(chart_buffer_test PRIVATE ETL_NO_STL)
add_test(NAME chart_buffer_test COMMAND chart_buffer_test)

add_executable(
    animation_inline_test
    animation_inline_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/cppsrc/U8g2lib.cpp
)
target_link_libraries(animation_inline_test PRIVATE pixelui u8g2_c)
target_compile_definitions(animation_inline_test PRIVATE ETL_NO_STL)
add_test(NAME animation_inline_test COMMAND animation_inline_test)

add_executable(
    popup_base_lifecycle_test
    popup_base_lifecycle_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/cppsrc/U8g2lib.cpp
)
target_link_libraries(popup_base_lifecycle_test PRIVATE pixelui u8g2_c)
target_compile_definitions(popup_base_lifecycle_test PRIVATE ETL_NO_STL)
add_test(NAME popup_base_lifecycle_test COMMAND popup_base_lifecycle_test)

add_executable(memory_metrics memory_metrics.cpp)
target_link_libraries(memory_metrics PRIVATE pixelui)

if(APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(
        application_stack_test
        PRIVATE
            "SHELL:-include ${CMAKE_CURRENT_SOURCE_DIR}/../include/core/etl_no_stl_compat.h"
    )
    target_compile_options(
        view_manager_lifecycle_test
        PRIVATE
            "SHELL:-include ${CMAKE_CURRENT_SOURCE_DIR}/../include/core/etl_no_stl_compat.h"
    )
    target_compile_options(
        chart_buffer_test
        PRIVATE
            "SHELL:-include ${CMAKE_CURRENT_SOURCE_DIR}/../include/core/etl_no_stl_compat.h"
    )
    target_compile_options(
        animation_inline_test
        PRIVATE
            "SHELL:-include ${CMAKE_CURRENT_SOURCE_DIR}/../include/core/etl_no_stl_compat.h"
    )
    target_compile_options(
        popup_base_lifecycle_test
        PRIVATE
            "SHELL:-include ${CMAKE_CURRENT_SOURCE_DIR}/../include/core/etl_no_stl_compat.h"
    )
endif()
