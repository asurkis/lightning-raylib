cmake_minimum_required(VERSION 3.5)

project(sandbox-raylib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sandbox-raylib main.cpp)

find_package(raylib CONFIG REQUIRED)
target_include_directories(sandbox-raylib PRIVATE ${raylib_INCLUDE_DIRS})
target_link_libraries(sandbox-raylib ${raylib_LIBRARIES})
