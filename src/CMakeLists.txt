cmake_minimum_required(VERSION 3.31)
project(kami)
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} test_lib) 
add_subdirectory(lib)
