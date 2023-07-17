
cmake_minimum_required (VERSION 3.9)


project(NinjaTimes VERSION 0.0.1 DESCRIPTION "Analyze a ninja build log.")


set (CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "Default C++ standard")
# Include sub-projects.
add_subdirectory ("src")
