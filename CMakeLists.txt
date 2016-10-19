cmake_minimum_required(VERSION 2.7)
project(corsika_reader CXX)
enable_language(C)

set(CMAKE_MACOSX_RPATH True)

set (Corsika_PACKAGE_NAME "corsika")

set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_CXX_FLAGS_DEBUG "-Wall -ggdb3 -O0 -fno-inline")
set (CMAKE_BUILD_TYPE Release)
set (CMAKE_CXX_FLAGS_DEBUG "-O3")

set (CMAKE_INCLUDE_CURRENT_DIR TRUE)
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

find_package (PythonLibs)
find_package (ZLIB REQUIRED)
find_package (BZip2 REQUIRED)
find_package (Boost REQUIRED COMPONENTS iostreams python unit_test_framework filesystem system)

message (STATUS "zlib library: ${ZLIB_LIBRARIES}")
message (STATUS "Boost lib dir: ${Boost_LIBRARY_DIRS}")
message (STATUS "Boost libraries: ${Boost_LIBRARIES}")
message (STATUS "Python library: ${PYTHON_LIBRARIES}")

add_definitions(
  -DCORSIKA_TEST_DIR="${CMAKE_SOURCE_DIR}/share/data"
  -DCORSIKA_EXAMPLE_DATA_DIR="${CMAKE_INSTALL_PREFIX}/share/${Corsika_PACKAGE_NAME}/data"
)

include_directories(${CMAKE_SOURCE_DIR}/include ${Boost_INCLUDE_DIR})

set (Corsika_HEADERS
  include/corsika/Constants.h
  include/corsika/CorsikaUnits.h
  include/corsika/CorsikaBlock.h
  include/corsika/RawStream.h
  include/corsika/RawCorsikaFile.h
  include/corsika/RawParticleIterator.h
  include/corsika/CorsikaIOException.h
  include/corsika/CorsikaParticle.h
  include/corsika/CorsikaShower.h
  include/corsika/CorsikaShowerFile.h
  include/corsika/CorsikaShowerFileParticleIterator.h
  include/corsika/ShowerParticleList.h
  include/corsika/GaisserHillasParameter.h
  include/corsika/particle/NucleusProperties.h
  include/corsika/particle/ParticleList.h
  include/corsika/particle/ParticleProperties.h
  include/corsika/particle/VParticleProperties.h
  include/corsika/CorsikaLongProfile.h
  include/corsika/CorsikaLongFile.h
  DESTINATION include
)

set (Corsika_SOURCES
  src/corsika/CorsikaBlock.cxx
  src/corsika/CorsikaParticle.cxx
  src/corsika/CorsikaShower.cxx
  src/corsika/CorsikaShowerFile.cxx
  src/corsika/CorsikaShowerFileParticleIterator.cxx
  src/corsika/GaisserHillasParameter.cxx
  src/corsika/RawStream.cxx
  src/corsika/RawCorsikaFile.cxx
  src/corsika/Index.cxx
  src/corsika/particle/NucleusProperties.cxx
  src/corsika/particle/ParticleProperties.cxx
  src/corsika/particle/ParticleList.cxx
  src/corsika/RawParticleIterator.cxx
  src/corsika/CorsikaLongFile.cxx
)


set (Corsika_BoostPython_SOURCES
  src/pybindings/CorsikaBlock_py.cxx
  src/pybindings/CorsikaParticle_py.cxx
  src/pybindings/CorsikaShower_py.cxx
  src/pybindings/CorsikaShowerFile_py.cxx
  src/pybindings/CorsikaShowerFileParticleIterator_py.cxx
  src/pybindings/CorsikaUnits_py.cxx
  src/pybindings/MathConstants_py.cxx
  src/pybindings/ParticleList_py.cxx
  src/pybindings/ParticleProperties_py.cxx
  src/pybindings/PhysicalConstants_py.cxx
  src/pybindings/RawCorsikaFile_py.cxx
  src/pybindings/RawParticleIterator_py.cxx
  src/pybindings/ShowerParticleList_py.cxx
  src/pybindings/CorsikaLongProfile_py.cxx
  src/pybindings/CorsikaLongFile_py.cxx
  src/pybindings/module.cxx
)

add_library (CorsikaReader SHARED
  ${Corsika_SOURCES}
)
target_link_libraries(CorsikaReader
  ${Boost_LIBRARIES}
  ${BZip2_LIBRARIES}
  ${ZLIB_LIBRARIES}
)

if (PYTHONLIBS_FOUND)
  include_directories (${PYTHON_INCLUDE_DIRS})
  add_library (${Corsika_PACKAGE_NAME} SHARED
    ${Corsika_BoostPython_SOURCES}
    )

  set_target_properties (${Corsika_PACKAGE_NAME} PROPERTIES
    PREFIX ""
    SUFFIX ".so"
    )

  target_link_libraries (${Corsika_PACKAGE_NAME}
    ${PYTHON_LIBRARIES}
    CorsikaReader
    )

  install(DIRECTORY share/examples/
    DESTINATION share/${Corsika_PACKAGE_NAME}/examples
    FILES_MATCHING PATTERN "*.py"
    		   PATTERN "*.cpp"
    		   PATTERN "*.txt"
		   PATTERN "README"
    )

  install (TARGETS ${Corsika_PACKAGE_NAME}
    LIBRARY DESTINATION lib
    )
endif (PYTHONLIBS_FOUND)

install (TARGETS CorsikaReader
  LIBRARY DESTINATION lib
)

install(FILES
  include/corsika/Index.h
  include/corsika/Constants.h
  include/corsika/CorsikaUnits.h
  include/corsika/CorsikaBlock.h
  include/corsika/RawStream.h
  include/corsika/RawCorsikaFile.h
  include/corsika/RawParticleIterator.h
  include/corsika/CorsikaIOException.h
  include/corsika/CorsikaParticle.h
  include/corsika/CorsikaShower.h
  include/corsika/CorsikaShowerFile.h
  include/corsika/CorsikaShowerFileParticleIterator.h
  include/corsika/ShowerParticleList.h
  include/corsika/GaisserHillasParameter.h
  DESTINATION include/corsika
)
install(FILES
  include/corsika/particle/NucleusProperties.h
  include/corsika/particle/ParticleList.h
  include/corsika/particle/ParticleProperties.h
  include/corsika/particle/VParticleProperties.h
  DESTINATION include/corsika/particle
)


add_executable(test_corsika
  test/main.cxx
  test/testRawStream.cxx
  test/testRawCorsikaFile.cxx
  test/testCorsikaFile.cxx
)

target_link_libraries(test_corsika CorsikaReader ${PYTHON_LIBRARIES})

enable_testing()
add_test(corsika test_corsika)
add_test(raw ${CMAKE_SOURCE_DIR}/share/examples/python/raw_example.py ${CMAKE_SOURCE_DIR}/share/data/DAT000002-32)
add_test(raw_2 ${CMAKE_SOURCE_DIR}/share/examples/python/raw_example_2.py ${CMAKE_SOURCE_DIR}/share/data/DAT000002-32)
add_test(read ${CMAKE_SOURCE_DIR}/share/examples/python/read_example.py ${CMAKE_SOURCE_DIR}/share/data/DAT000002-32)
add_test(ehistory ${CMAKE_SOURCE_DIR}/share/examples/python/check_ehistory.py ${CMAKE_SOURCE_DIR}/share/data/DAT000011-proton-EHISTORY-MUPROD)
add_test(high_low ${CMAKE_SOURCE_DIR}/share/examples/python/compare_high_low.py ${CMAKE_SOURCE_DIR}/share/data/DAT000011-proton-EHISTORY-MUPROD)

configure_file(docs/Doxyfile docs/Doxyfile)
add_custom_target(docs doxygen ${CMAKE_BINARY_DIR}/docs/Doxyfile
  COMMAND ${CMAKE_COMMAND} -E env DOXYGEN_XML=${CMAKE_BINARY_DIR}/docs/xml sphinx-build -b html ${CMAKE_SOURCE_DIR}/docs docs/html
  COMMENT building documentation
  )

SET(FILE         corsika_example_data)
SET(SOURCE       http://www.bartol.udel.edu/~javierg)
SET(TO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/share)

OPTION(FETCH_CORSIKA_DATA "Fetch the example data" ON)
FIND_PACKAGE(Wget)
IF (WGET_FOUND)
  IF (FETCH_CORSIKA_DATA)
    IF (NOT EXISTS ${TO_DIR}/data)
      message(STATUS "Downloading and unpacking example CORSIKA data.")
      execute_process (COMMAND rm -f * WORKING_DIRECTORY ${TO_DIR}/${FILE})
      execute_process (COMMAND rmdir ${FILE} WORKING_DIRECTORY ${TO_DIR})
      execute_process (COMMAND ${WGET_EXECUTABLE} -P corsika_example_data -nd -r --no-parent ${SOURCE}/${FILE}/ WORKING_DIRECTORY ${TO_DIR})
      file (RENAME ${TO_DIR}/${FILE} ${TO_DIR}/data)
    ELSE (NOT EXISTS ${TO_DIR}/data)
      message(STATUS "${TO_DIR}/data already exists")
    ENDIF (NOT EXISTS ${TO_DIR}/data)
  ELSE (FETCH_CORSIKA_DATA)
    message(STATUS "Not downloading CORSIKA example data.")
  ENDIF (FETCH_CORSIKA_DATA)
  IF (EXISTS ${TO_DIR}/data)
    install (DIRECTORY ${TO_DIR}/data DESTINATION share/${Corsika_PACKAGE_NAME} FILES_MATCHING PATTERN "DAT*")
  ENDIF (EXISTS ${TO_DIR}/data)
ELSE (WGET_FOUND)
  message(STATUS "WGet not found. CORSIKA example data will not be available.")
ENDIF (WGET_FOUND)
