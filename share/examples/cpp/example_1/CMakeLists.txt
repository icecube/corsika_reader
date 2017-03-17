cmake_minimum_required(VERSION 2.7)
project(corsika_example CXX)

add_definitions(-std=c++0x)

# the following was needed in my mac. Uncomment it if you get a missing python symbol
#find_package (PythonLibs)
set (CORSIKA_READER_HOME "@CMAKE_INSTALL_PREFIX@")
find_library (CORSIKA_READER_LIBRARY CorsikaReader PATHS ${CORSIKA_READER_HOME}/lib)
set (CORSIKA_READER_INCLUDE_DIR ${CORSIKA_READER_HOME}/include)

#find_package (Boost REQUIRED COMPONENTS iostreams python unit_test_framework)

# if you use Boost, you would add ${Boost_INCLUDE_DIR} in the following
include_directories(${CORSIKA_READER_INCLUDE_DIR} ${Boost_INCLUDE_DIR})

add_executable (corsika_example_1 main.cpp)

# if you use Boost, you would add ${Boost_LIBRARIES} in the following
target_link_libraries(corsika_example_1
  ${CORSIKA_READER_HOME}/lib/libCorsikaReader.dylib
  ${PYTHON_LIBRARIES}
)
