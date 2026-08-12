add_executable(application_stack_test application_stack_test.cpp)
target_link_libraries(application_stack_test PRIVATE pixelui)
target_compile_definitions(application_stack_test PRIVATE ETL_NO_STL)
add_test(NAME application_stack_test COMMAND application_stack_test)

add_executable(
    application_stack_exception_test
    application_stack_exception_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/app/ApplicationStack.cpp
)
target_include_directories(
    application_stack_exception_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/etl/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/cppsrc
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/csrc
)
target_compile_definitions(application_stack_exception_test PRIVATE ETL_NO_STL)
if(APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(
        application_stack_exception_test
        PRIVATE
            "SHELL:-include ${CMAKE_CURRENT_SOURCE_DIR}/../include/core/etl_no_stl_compat.h"
    )
endif()
add_test(NAME application_stack_exception_test COMMAND application_stack_exception_test)

add_executable(
    view_manager_lifecycle_test
    view_manager_lifecycle_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/u8g2/cppsrc/U8g2lib.cpp
)
target_link_libraries(view_manager_lifecycle_test PRIVATE pixelui u8g2_c)
target_compile_definitions(view_manager_lifecycle_test PRIVATE ETL_NO_STL)
add_test(NAME view_manager_lifecycle_test COMMAND view_manager_lifecycle_test)

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
endif()
