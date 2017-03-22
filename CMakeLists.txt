cmake_minimum_required(VERSION 2.7)
project(corsika_reader CXX)
enable_language(C)

set(CMAKE_MACOSX_RPATH True)
# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")

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
find_package (Boost REQUIRED COMPONENTS python unit_test_framework filesystem system)

message (STATUS "zlib library: ${ZLIB_LIBRARIES}")
message (STATUS "BZip2 library: ${BZIP2_LIBRARIES}")
message (STATUS "Boost lib dir: ${Boost_LIBRARY_DIRS}")
message (STATUS "Boost libraries: ${Boost_LIBRARIES}")
message (STATUS "Python library: ${PYTHON_LIBRARIES}")

add_definitions(
  -DCORSIKA_TEST_DIR="${CMAKE_SOURCE_DIR}/share/data"
  -DCORSIKA_EXAMPLE_DATA_DIR="${CMAKE_INSTALL_PREFIX}/share/${Corsika_PACKAGE_NAME}/data"
)
add_definitions(-std=c++0x)


include_directories(${CMAKE_SOURCE_DIR}/include ${Boost_INCLUDE_DIR})

set (Corsika_HEADERS
  include/corsika/Constants.h
  include/corsika/Units.h
  include/corsika/Block.h
  include/corsika/RawStream.h
  include/corsika/RawParticleStream.h
  include/corsika/IOException.h
  include/corsika/Particle.h
  include/corsika/Shower.h
  include/corsika/ShowerFile.h
  include/corsika/ShowerParticleStream.h
  include/corsika/GaisserHillasParameter.h
  include/corsika/particle/NucleusProperties.h
  include/corsika/particle/ParticleList.h
  include/corsika/particle/ParticleProperties.h
  include/corsika/particle/VParticleProperties.h
  include/corsika/LongProfile.h
  include/corsika/LongFile.h
  include/corsika/FileStream.h
  include/corsika/Logging.h
  DESTINATION include
)

set (Corsika_SOURCES
  src/corsika/Block.cxx
  src/corsika/Particle.cxx
  src/corsika/Shower.cxx
  src/corsika/ShowerFile.cxx
  src/corsika/ShowerParticleStream.cxx
  src/corsika/GaisserHillasParameter.cxx
  src/corsika/RawStream.cxx
  src/corsika/Index.cxx
  src/corsika/particle/NucleusProperties.cxx
  src/corsika/particle/ParticleList.cxx
  src/corsika/RawParticleStream.cxx
  src/corsika/LongFile.cxx
  src/corsika/FileStream.cxx
  src/corsika/FileIndex.cxx
  src/corsika/Logging.cxx
)


set (Corsika_BoostPython_SOURCES
  src/pybindings/Block_py.cxx
  src/pybindings/Particle_py.cxx
  src/pybindings/Shower_py.cxx
  src/pybindings/ShowerFile_py.cxx
  src/pybindings/ShowerFileParticleIterator_py.cxx
  src/pybindings/Units_py.cxx
  src/pybindings/MathConstants_py.cxx
  src/pybindings/ParticleList_py.cxx
  src/pybindings/ParticleProperties_py.cxx
  src/pybindings/PhysicalConstants_py.cxx
  src/pybindings/RawParticleIterator_py.cxx
  src/pybindings/LongProfile_py.cxx
  src/pybindings/LongFile_py.cxx
  src/pybindings/RawStream_py.cxx
  src/pybindings/module.cxx
)

add_library (CorsikaReader SHARED
  ${Corsika_SOURCES}
)
target_link_libraries(CorsikaReader
  ${Boost_LIBRARIES}
  ${BZIP2_LIBRARIES}
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
  include/corsika/Units.h
  include/corsika/Block.h
  include/corsika/RawStream.h
  include/corsika/RawParticleStream.h
  include/corsika/IOException.h
  include/corsika/Particle.h
  include/corsika/Shower.h
  include/corsika/ShowerFile.h
  include/corsika/ShowerParticleStream.h
  include/corsika/GaisserHillasParameter.h
  include/corsika/Logging.h
  DESTINATION include/corsika
)
install(FILES
  include/corsika/particle/NucleusProperties.h
  include/corsika/particle/ParticleList.h
  include/corsika/particle/ParticleProperties.h
  include/corsika/particle/VParticleProperties.h
  DESTINATION include/corsika/particle
)
install(DIRECTORY share/data DESTINATION share/corsika)


add_executable(test_corsika
  test/test_corsika.cxx
)

target_link_libraries(test_corsika CorsikaReader ${PYTHON_LIBRARIES})

enable_testing()
add_test(corsika test_corsika)
add_test(raw ${CMAKE_SOURCE_DIR}/share/examples/python/raw_example.py ${CMAKE_SOURCE_DIR}/share/data/DAT000002-32)
add_test(raw_2 ${CMAKE_SOURCE_DIR}/share/examples/python/raw_example_2.py ${CMAKE_SOURCE_DIR}/share/data/DAT000002-32)
add_test(read ${CMAKE_SOURCE_DIR}/share/examples/python/read_example.py ${CMAKE_SOURCE_DIR}/share/data/DAT000002-32)
add_test(ehistory ${CMAKE_SOURCE_DIR}/share/examples/python/check_ehistory.py ${CMAKE_SOURCE_DIR}/share/data/DAT000011-proton-EHISTORY-MUPROD)

add_test(raw python ${CMAKE_SOURCE_DIR}/test/python/raw.py --input ${CMAKE_SOURCE_DIR}/share/data/DAT000002-32)
add_test(read python ${CMAKE_SOURCE_DIR}/test/python/read.py --input ${CMAKE_SOURCE_DIR}/share/data/DAT000002-32)
add_test(ehistory python ${CMAKE_SOURCE_DIR}/test/python/ehistory.py --input ${CMAKE_SOURCE_DIR}/share/data/DAT000011-proton-EHISTORY-MUPROD)
add_test(high_low ${CMAKE_SOURCE_DIR}/test/python/high_low.py --input ${CMAKE_SOURCE_DIR}/share/data/DAT000011-proton-EHISTORY-MUPROD)

configure_file(docs/Doxyfile docs/Doxyfile)
add_custom_target(docs doxygen ${CMAKE_BINARY_DIR}/docs/Doxyfile
  COMMAND ${CMAKE_COMMAND} -E env DOXYGEN_XML=${CMAKE_BINARY_DIR}/docs/xml sphinx-build -b html ${CMAKE_SOURCE_DIR}/docs docs/html
  COMMENT building documentation
  )
