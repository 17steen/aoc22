cmake_minimum_required(VERSION 3.15)
project(day14)


add_executable(${PROJECT_NAME} main.cc)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

include("~/vcpkg/scripts/buildsystems/vcpkg.cmake")

find_path(FASTIO_INCLUDE_DIRS "fast_io.h")
target_include_directories(${PROJECT_NAME} PRIVATE ${FASTIO_INCLUDE_DIRS})
find_package(ctre CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ctre::ctre)

find_package(range-v3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE range-v3 range-v3-meta range-v3::meta range-v3-concepts)
find_package(simdjson CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE simdjson::simdjson)

#target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=undefined -fsanitize-trap=undefined)
#target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=undefined -fsanitize-trap=undefined)

