cmake_minimum_required(VERSION 2.6)
project(data2vtu)


add_executable(data2vtu data2vtu.cpp)

#enable_testing()
#include(CTest)
#add_subdirectory(Tests)


install(TARGETS data2vtu EXPORT data2vtu-targets RUNTIME DESTINATION bin)
