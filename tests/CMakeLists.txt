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

add_executable(
    popup_pool_test
    popup_pool_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/cppsrc/U8g2lib.cpp
)
target_link_libraries(popup_pool_test PRIVATE pixelui u8g2_c)
target_compile_definitions(popup_pool_test PRIVATE ETL_NO_STL)
add_test(NAME popup_pool_test COMMAND popup_pool_test)

add_executable(
    popup_value_digits_test
    popup_value_digits_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/cppsrc/U8g2lib.cpp
)
target_link_libraries(popup_value_digits_test PRIVATE pixelui u8g2_c)
target_compile_definitions(popup_value_digits_test PRIVATE ETL_NO_STL)
add_test(NAME popup_value_digits_test COMMAND popup_value_digits_test)

add_executable(
    focus_manager_transition_test
    focus_manager_transition_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/cppsrc/U8g2lib.cpp
)
target_link_libraries(focus_manager_transition_test PRIVATE pixelui u8g2_c)
target_compile_definitions(focus_manager_transition_test PRIVATE ETL_NO_STL)
add_test(NAME focus_manager_transition_test COMMAND focus_manager_transition_test)

add_executable(
    widget_tree_test
    widget_tree_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/cppsrc/U8g2lib.cpp
)
target_link_libraries(widget_tree_test PRIVATE pixelui u8g2_c)
target_compile_definitions(widget_tree_test PRIVATE ETL_NO_STL)
add_test(NAME widget_tree_test COMMAND widget_tree_test)

add_executable(
    label_font_test
    label_font_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/cppsrc/U8g2lib.cpp
)
target_link_libraries(label_font_test PRIVATE pixelui u8g2_c)
target_compile_definitions(label_font_test PRIVATE ETL_NO_STL)
add_test(NAME label_font_test COMMAND label_font_test)

add_executable(
    render_scheduling_test
    render_scheduling_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/cppsrc/U8g2lib.cpp
)
target_link_libraries(render_scheduling_test PRIVATE pixelui u8g2_c)
target_compile_definitions(render_scheduling_test PRIVATE ETL_NO_STL)
add_test(NAME render_scheduling_test COMMAND render_scheduling_test)

add_executable(
    canvas_camera_test
    canvas_camera_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/cppsrc/U8g2lib.cpp
)
target_link_libraries(canvas_camera_test PRIVATE pixelui u8g2_c)
target_compile_definitions(canvas_camera_test PRIVATE ETL_NO_STL)
add_test(NAME canvas_camera_test COMMAND canvas_camera_test)

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
    target_compile_options(
        popup_pool_test
        PRIVATE
            "SHELL:-include ${CMAKE_CURRENT_SOURCE_DIR}/../include/core/etl_no_stl_compat.h"
    )
    target_compile_options(
        popup_value_digits_test
        PRIVATE
            "SHELL:-include ${CMAKE_CURRENT_SOURCE_DIR}/../include/core/etl_no_stl_compat.h"
    )
    target_compile_options(
        focus_manager_transition_test
        PRIVATE
            "SHELL:-include ${CMAKE_CURRENT_SOURCE_DIR}/../include/core/etl_no_stl_compat.h"
    )
    target_compile_options(
        widget_tree_test
        PRIVATE
            "SHELL:-include ${CMAKE_CURRENT_SOURCE_DIR}/../include/core/etl_no_stl_compat.h"
    )
    target_compile_options(
        label_font_test
        PRIVATE
            "SHELL:-include ${CMAKE_CURRENT_SOURCE_DIR}/../include/core/etl_no_stl_compat.h"
    )
    target_compile_options(
        render_scheduling_test
        PRIVATE
            "SHELL:-include ${CMAKE_CURRENT_SOURCE_DIR}/../include/core/etl_no_stl_compat.h"
    )
    target_compile_options(
        canvas_camera_test
        PRIVATE
            "SHELL:-include ${CMAKE_CURRENT_SOURCE_DIR}/../include/core/etl_no_stl_compat.h"
    )
endif()
