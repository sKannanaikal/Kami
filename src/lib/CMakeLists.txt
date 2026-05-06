add_library(elf_lib SHARED elf.cpp)
target_include_directories(elf_lib
	INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}
)
