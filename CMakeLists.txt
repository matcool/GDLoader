cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(gd-loader)

add_library(${PROJECT_NAME} SHARED src/GDLoader.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC libraries/minhook/include)
add_subdirectory(libraries/minhook)
target_link_libraries(${PROJECT_NAME} minhook)
target_compile_definitions(${PROJECT_NAME} PRIVATE "GDLOADER_EXPORT=1")