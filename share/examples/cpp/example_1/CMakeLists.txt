cmake_minimum_required(VERSION 2.7)
project(corsika_example CXX)

set (CORSIKA_READER_HOME /data/user/jgonzalez/CorsikaReader-install)

#find_package (Boost REQUIRED COMPONENTS iostreams python unit_test_framework)

# if you use Boost, you would add ${Boost_INCLUDE_DIR} in the following
include_directories(${CORSIKA_READER_HOME}/include ${Boost_INCLUDE_DIR})

add_executable (corsika_example_1 main.cpp)

# if you use Boost, you would add ${Boost_LIBRARIES} in the following
target_link_libraries(corsika_example_1
  ${CORSIKA_READER_HOME}/lib/libCorsikaReader.so
)
