cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_COMPILER "/opt/homebrew/bin/g++-13")

project(Haversine LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Collect source files (you might want to exclude main.cpp for the library)
file(GLOB CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list(REMOVE_ITEM CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# Create a library target with your core code
add_library(core ${CORE_SOURCES})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Create your main executable (if needed), linking with the core library
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE core)

############ GoogleTest Setup ############
enable_testing()
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Collect test source files from the tests directory
file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")

# Create a test executable and link it with GoogleTest and with your core library
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE gtest_main core)

include(GoogleTest)
gtest_discover_tests(tests)
