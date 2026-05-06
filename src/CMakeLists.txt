cmake_minimum_required(VERSION 3.31)
project(kami)
add_executable(${PROJECT_NAME} main.cpp)

include(FetchContent)
FetchContent_Declare(
  boostlib
  URL https://github.com/boostorg/boost/releases/download/boost-1.91.0-1/boost-1.91.0-1-cmake.zip
)
FetchContent_MakeAvailable(boostlib)

target_link_libraries(${PROJECT_NAME} elf_lib Boost::program_options) 

add_subdirectory(lib)
