cmake_minimum_required(VERSION 3.15)
project(day08)

add_executable(${PROJECT_NAME} main.cc)

#target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined)
#target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined)
