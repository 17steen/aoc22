cmake_minimum_required(VERSION 3.15)
project(day06_futhark)

add_custom_command(
     OUTPUT
         ${CMAKE_CURRENT_BINARY_DIR}/lib.c
         ${CMAKE_CURRENT_BINARY_DIR}/lib.h
     COMMAND futhark cuda 
         --library lib.fut 
         -o ${CMAKE_CURRENT_BINARY_DIR}/lib
     DEPENDS lib.fut
     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
     VERBATIM
)

add_library(lib SHARED
   ${CMAKE_CURRENT_BINARY_DIR}/lib.c
   ${CMAKE_CURRENT_BINARY_DIR}/lib.h
)

add_executable(${PROJECT_NAME} main.cc)
target_link_libraries(${PROJECT_NAME} PRIVATE lib)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

include("~/vcpkg/scripts/buildsystems/vcpkg.cmake")

find_path(FASTIO_INCLUDE_DIRS "fast_io.h")
target_include_directories(${PROJECT_NAME} PRIVATE ${FASTIO_INCLUDE_DIRS})


find_package(CUDAToolkit REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CUDA::cudart CUDA::nvrtc)
find_package(OpenCL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)
