#
# MIT License
# 
# Copyright (c) 2026 USRA Technologies
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
# 
#

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
