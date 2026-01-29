# Find cmocka
find_package(PkgConfig REQUIRED)
pkg_check_modules(CMOCKA REQUIRED cmocka)

# Project root directory
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Test sources
set(TEST_SOURCES
    nomcu/test_nomcu_gpio.c
    assert_wrap.c
    ${PROJECT_ROOT}/nomcu/nomcu_gpio.c
)

# Create test executable
add_executable(test_nomcu_gpio ${TEST_SOURCES})

# Ensure asserts are enabled (undefine NDEBUG) and disable _FORTIFY_SOURCE
target_compile_definitions(test_nomcu_gpio PRIVATE
    _FORTIFY_SOURCE=0
)

# Disable optimizations and ensure NDEBUG is undefined so asserts work
target_compile_options(test_nomcu_gpio PRIVATE
    -U_FORTIFY_SOURCE
    -UNDEBUG
    -O0
)

# Use linker wrap to intercept __assert_fail for cmocka expect_assert_failure
target_link_options(test_nomcu_gpio PRIVATE
    -Wl,--wrap=__assert_fail
)

target_link_libraries(test_nomcu_gpio 
    PRIVATE 
        ${CMOCKA_LIBRARIES}
)

target_include_directories(test_nomcu_gpio 
    PRIVATE 
        ${PROJECT_ROOT}
        ${PROJECT_ROOT}/nomcu
        ${CMOCKA_INCLUDE_DIRS}
)

# Add test to CTest
add_test(NAME test_nomcu_gpio COMMAND test_nomcu_gpio)

# Set test properties
set_tests_properties(test_nomcu_gpio PROPERTIES
    TIMEOUT 30
)
