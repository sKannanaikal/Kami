cmake_minimum_required(VERSION 3.31)
project(kami_test)

# Google Testing framework requires a minimum of c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Telling cmake to get the Google test library
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/d72f9c8aea6817cdf1ca0ac10887f328de7f3da2.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

# So in the testing project the kami_test is the new project main binary
add_executable(kami_test kami_test.cpp)

# Link in the gtest library and source and eventually kami source as well as a library
target_link_libraries(kami_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(kami_test)
