cmake_minimum_required(VERSION 3.7...3.20 FATAL_ERROR)
project(FileServer CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler options
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2 -pthread")
endif()

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_options(-g -ggdb3)
endif()

option(BUILD_TESTS "Enable building tests" OFF)

# Include directories
include_directories(include)

# Find required packages
find_package(Threads REQUIRED)

# Add subdirectories
add_subdirectory(src)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
    add_subdirectory(benchmarks)
endif()

set(INSTALL_DIRECTORY
        ${PROJECT_SOURCE_DIR}/bin
        CACHE PATH
        "Install directory")
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
file(MAKE_DIRECTORY ${INSTALL_DIRECTORY})
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY
        ${INSTALL_DIRECTORY}/bin
        CACHE PATH
        "Single Directory for all"
        )

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${INSTALL_DIRECTORY}/bin
        CACHE PATH
        "Single Directory for all"
        )
        
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        ${INSTALL_DIRECTORY}/bin
        CACHE PATH
        "Single Directory for all"
        )
install(TARGETS file_server file_client DESTINATION ${INSTALL_DIRECTORY})
