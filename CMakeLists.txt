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
cmake_minimum_required(VERSION 3.14)
project(mcu-interface 
    VERSION 0.1.0
    DESCRIPTION "MCU vendor-agnostic interface"
    LANGUAGES C
)

# Options
option(BUILD_TESTS "Build tests" ON)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Library sources
set(NOMCU_SOURCES
    nomcu/nomcu_gpio.c
)

# Create library
add_library(mcui_nomcu STATIC ${NOMCU_SOURCES})
target_include_directories(mcui_nomcu PUBLIC ${CMAKE_SOURCE_DIR})

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
