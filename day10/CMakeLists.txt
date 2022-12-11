cmake_minimum_required(VERSION 3.15)
project(day08)

add_executable(${PROJECT_NAME} main.cc)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

include("~/vcpkg/scripts/buildsystems/vcpkg.cmake")

find_path(FASTIO_INCLUDE_DIRS "fast_io.h")
target_include_directories(${PROJECT_NAME} PRIVATE ${FASTIO_INCLUDE_DIRS})
#target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined)
#target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined)
