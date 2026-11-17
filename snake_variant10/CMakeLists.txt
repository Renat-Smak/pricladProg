cmake_minimum_required(VERSION 3.20)
project(SnakeVariant10 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(snake_core
    src/game.cpp
    src/states.cpp
)
target_include_directories(snake_core PUBLIC include)

if(MSVC)
    target_compile_options(snake_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(snake_core PRIVATE -Wall -Wextra -Wpedantic -Wconversion)
endif()

add_executable(snake_game src/main.cpp)
target_link_libraries(snake_game PRIVATE snake_core)

include(CTest)
if(BUILD_TESTING)
    add_executable(snake_tests tests/tests.cpp)
    target_link_libraries(snake_tests PRIVATE snake_core)
    add_test(NAME snake_tests COMMAND snake_tests)
endif()
