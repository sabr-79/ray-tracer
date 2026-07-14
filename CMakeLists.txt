cmake_minimum_required(VERSION 3.10)
project(RayTracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(raytracer main.cpp)
target_compile_options(raytracer PRIVATE -O3 -march=native -ffast-math -flto)
target_link_libraries(raytracer PRIVATE "-framework Dispatch")
target_include_directories(raytracer PRIVATE src)