cmake_minimum_required(VERSION 3.15)
project(day02)

add_executable(${PROJECT_NAME} main.cc)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

include("~/vcpkg/scripts/buildsystems/vcpkg.cmake")

find_package(OpenMP REQUIRED)
find_path(FASTIO_INCLUDE_DIRS "fast_io.h")
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wnon-virtual-dtor -fopenmp)
target_include_directories(${PROJECT_NAME} PRIVATE ${FASTIO_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)