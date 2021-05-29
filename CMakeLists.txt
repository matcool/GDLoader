cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_NAME gd-loader)
project(${PROJECT_NAME})

add_library(${PROJECT_NAME} SHARED src/GDLoader.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC libraries/minhook/include)
add_subdirectory(libraries/minhook)
target_link_libraries(${PROJECT_NAME} minhook)
