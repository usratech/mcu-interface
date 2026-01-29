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
