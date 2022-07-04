# CMakeList.txt: xlcomm/base/test 的 CMake 项目，在此处包括源代码并定义项目特定的逻辑。

cmake_minimum_required(VERSION 3.8)

add_executable(thread_test thread_test.cc)
target_link_libraries(thread_test xlcomm_base)