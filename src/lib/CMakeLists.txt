add_library(test_lib SHARED test.cpp)
target_include_directories(test_lib
	INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR})
