# CMakeList.txt: xlcomm/base 的 CMake 项目，在此处包括源代码并定义项目特定的逻辑。
cmake_minimum_required(VERSION 3.8)

aux_source_directory(. BASE_SRC)
add_library(xlcomm_base SHARED ${BASE_SRC})
if(CMAKE_HOST_WIN32)
  target_compile_definitions(xlcomm_base PRIVATE XLCOMM_EXPORT)
endif()

add_subdirectory(test)
