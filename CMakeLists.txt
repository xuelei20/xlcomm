# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置并包含子项目。
cmake_minimum_required(VERSION 3.8)

project(xlcomm C CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CXX_FLAGS
 -g
 # -DVALGRIND
 -DCHECK_PTHREAD_RETURN_VALUE
 -D_FILE_OFFSET_BITS=64
 -Wall
# -Wextra
# -Werror
#  -Wconversion
# -Wno-unused-parameter
#  -Wold-style-cast
#  -Woverloaded-virtual
# -Wpointer-arith
# -Wshadow
# -Wwrite-strings
# -march=native
 # -MMD
 -std=c++14
 -rdynamic
)
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/third_party/boost_1_77_0)

link_directories(${PROJECT_SOURCE_DIR}/third_party/lib)

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

add_subdirectory(xlcomm/base)