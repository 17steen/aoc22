cmake_minimum_required(VERSION 3.15)
project(day20)


add_executable(${PROJECT_NAME} main.cc)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

target_compile_options(${PROJECT_NAME} PRIVATE "-Wconversion" "-Wall" "-Wextra" "-Wpedantic" "-ffast-math")

include("~/vcpkg/scripts/buildsystems/vcpkg.cmake")

find_path(FASTIO_INCLUDE_DIRS "fast_io.h")
target_include_directories(${PROJECT_NAME} PRIVATE ${FASTIO_INCLUDE_DIRS})
find_package(ctre CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ctre::ctre)

find_package(Boost 1.80.0 REQUIRED COMPONENTS)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::boost)

find_package(PkgConfig REQUIRED)
pkg_check_modules(gmpxx REQUIRED IMPORTED_TARGET gmpxx)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::gmpxx)

target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=undefined)
target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=undefined)


