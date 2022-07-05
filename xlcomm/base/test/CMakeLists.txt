# CMakeList.txt: xlcomm/base/test 的 CMake 项目，在此处包括源代码并定义项目特定的逻辑。
cmake_minimum_required(VERSION 3.8)

set(TARGET_LINK_LIBS xlcomm_base boost_unit_test_framework)

if(NOT CMAKE_HOST_WIN32)
  set(TARGET_LINK_LIBS ${TARGET_LINK_LIBS} pthread)
endif()

add_executable(thread_test thread_test.cc)
target_link_libraries(thread_test ${TARGET_LINK_LIBS})