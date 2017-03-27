cmake_minimum_required(VERSION 2.7)
project(corsika_example CXX)

add_definitions(-std=c++0x)

set (CorsikaReader_DIR @CMAKE_INSTALL_PREFIX@/share/corsika)
find_package (CorsikaReader REQUIRED)
include_directories(${CorsikaReader_INCLUDE_DIRS})
add_executable (corsika_example_1 main.cpp)

target_link_libraries(corsika_example_1
  ${CorsikaReader_LIBRARIES}
)
