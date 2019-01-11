cmake_minimum_required(VERSION 2.7)
project(corsika_reader CXX)
enable_language(C)

set (CMAKE_MACOSX_RPATH True)
set (CMAKE_SKIP_BUILD_RPATH  FALSE)
set (CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
  if(APPLE)
    set (CMAKE_INSTALL_RPATH @loader_path)
  elseif(UNIX)
    set (CMAKE_INSTALL_RPATH "\$ORIGIN")
  else()
    set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  endif()
ENDIF("${isSystemDir}" STREQUAL "-1")

set (Corsika_PACKAGE_NAME "corsika")

set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_CXX_FLAGS_DEBUG "-Wall -ggdb3 -O0 -fno-inline")
set (CMAKE_BUILD_TYPE Release)
set (CMAKE_CXX_FLAGS_DEBUG "-O3")

set (CMAKE_INCLUDE_CURRENT_DIR TRUE)
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

find_program (PYTHON_CONFIG "python-config")
option (CORSIKA_IGNORE_PYTHON_CONFIG "Do not try to use python-config" FALSE)
if (PYTHON_CONFIG AND NOT CORSIKA_IGNORE_PYTHON_CONFIG)
  message(STATUS "Python config found at ${PYTHON_CONFIG}. Using whatever it says.")
  execute_process(COMMAND "${PYTHON_CONFIG}" "--ldflags" RESULT_VARIABLE _lib_result_ OUTPUT_VARIABLE _output_ ERROR_VARIABLE _error_)
  if (_lib_result_ EQUAL 0)
    string (STRIP "${_output_}" _a_)
    string (REPLACE "-l" "" _a_ ${_a_})
    string (REPLACE "-L" "" _a_ ${_a_})
    string (REPLACE " " ";" _a_ ${_a_})
    list (GET _a_ 0 _dir_)
    list (GET _a_ 1 _lib_)
    find_library (PYTHON_LIBRARY ${_lib_} HINTS ${_dir_} NO_DEFAULT_PATH DOC "Python library given by python-config.")
  else ()
    message (WARNING "python-config error (${_lib_result_}):\n- Output: ${_output_}\n- Error: ${_error_}")
  endif ()
  execute_process(COMMAND "${PYTHON_CONFIG}" "--includes" RESULT_VARIABLE _inc_result_ OUTPUT_VARIABLE _output_ ERROR_VARIABLE _error_)
  if (_inc_result_ EQUAL 0)
    string (STRIP "${_output_}" _a_)
    string (REPLACE "-I" "" _a_ ${_a_})
    string (REPLACE " " ";" _a_ ${_a_})
    list (GET _a_ 0 _dir_)
    set (PYTHON_INCLUDE_DIR "${_dir_}" CACHE STRING "Python includes directory given by python-config." FORCE)
  else ()
    message (WARNING "python-config error (${_inc_result_}):\n- Output: ${_output_}\n- Error: ${_error_}")
  endif ()
  if (NOT _lib_result_ EQUAL 0 OR NOT _lib_result_ EQUAL 0)
    message (SEND_ERROR "python-config error. See above.")
  endif()
endif ()

#find_package (Python COMPONENTS Interpreter Development)
find_package(PythonInterp) # this is to make the libraries consistent with the interpreter
find_package (PythonLibs)
find_package (ZLIB REQUIRED)
find_package (BZip2 REQUIRED)
find_package (Boost REQUIRED COMPONENTS python filesystem system)

message (STATUS "zlib library: ${ZLIB_LIBRARIES}")
message (STATUS "BZip2 library: ${BZIP2_LIBRARIES}")
message (STATUS "Boost lib dir: ${Boost_LIBRARY_DIRS}")
message (STATUS "Boost libraries: ${Boost_LIBRARIES}")
message (STATUS "Python executable:  ${PYTHON_EXECUTABLE}")
message (STATUS "Python includes:    ${PYTHON_INCLUDE_DIRS}")
message (STATUS "Python libraries:   ${PYTHON_LIBRARIES}")
message (STATUS "Python libs version: ${PYTHONLIBS_VERSION_STRING}")
message (STATUS "Python version: ${PYTHON_VERSION_STRING}")

if (NOT ${PYTHON_VERSION_STRING} VERSION_EQUAL ${PYTHONLIBS_VERSION_STRING})
  message (SEND_ERROR "Python interpreter and libraries have different versions (${PYTHON_VERSION_STRING} != ${PYTHONLIBS_VERSION_STRING}). Perhaps you need to specify PYTHON_INCLUDE_DIR and PYTHON_LIBRARY.")
endif (NOT ${PYTHON_VERSION_STRING} VERSION_EQUAL ${PYTHONLIBS_VERSION_STRING})

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
    PATTERN "README"
    )
  configure_file (${CMAKE_SOURCE_DIR}/share/examples/cpp/example_1/CMakeLists.txt ${CMAKE_BINARY_DIR}/share/examples/cpp/example_1/CMakeLists.txt @ONLY)
  install (FILES ${CMAKE_BINARY_DIR}/share/examples/cpp/example_1/CMakeLists.txt
    DESTINATION share/${Corsika_PACKAGE_NAME}/examples/cpp/example_1
    )

  install (TARGETS ${Corsika_PACKAGE_NAME}
    LIBRARY DESTINATION lib
    )
endif (PYTHONLIBS_FOUND)

install (TARGETS CorsikaReader
  LIBRARY DESTINATION lib
)

configure_file (${CMAKE_SOURCE_DIR}/share/CorsikaReaderConfig.cmake ${CMAKE_BINARY_DIR}/share/CorsikaReaderConfig.cmake @ONLY)
install (FILES ${CMAKE_BINARY_DIR}/share/CorsikaReaderConfig.cmake
  DESTINATION share/${Corsika_PACKAGE_NAME}
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
  include/corsika/LongFile.h
  include/corsika/LongProfile.h
  include/corsika/FileIndex.h
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
  test/main.cxx
  test/test_file.cxx
  test/test_low_high.cxx
  test/test_rawstream.cxx
  test/test_index.cxx
)

target_link_libraries(test_corsika CorsikaReader ${PYTHON_LIBRARIES})

enable_testing()
add_test(corsika test_corsika ${CMAKE_SOURCE_DIR}/share/data)
#add_test(raw ${CMAKE_SOURCE_DIR}/share/examples/python/raw_example.py ${CMAKE_SOURCE_DIR}/share/data/DAT000002-32)
add_test(raw_2 ${CMAKE_SOURCE_DIR}/share/examples/python/raw_example_2.py ${CMAKE_SOURCE_DIR}/share/data/DAT000002-32)
#add_test(read ${CMAKE_SOURCE_DIR}/share/examples/python/read_example.py ${CMAKE_SOURCE_DIR}/share/data/DAT000002-32)
#add_test(ehistory ${CMAKE_SOURCE_DIR}/share/examples/python/check_ehistory.py ${CMAKE_SOURCE_DIR}/share/data/DAT000011-proton-EHISTORY-MUPROD)

add_test(raw python ${CMAKE_SOURCE_DIR}/test/python/raw.py --input ${CMAKE_SOURCE_DIR}/share/data/DAT000002-32)
add_test(read python ${CMAKE_SOURCE_DIR}/test/python/read.py --input ${CMAKE_SOURCE_DIR}/share/data/DAT000002-32)
add_test(ehistory python ${CMAKE_SOURCE_DIR}/test/python/ehistory.py --input ${CMAKE_SOURCE_DIR}/share/data/DAT000011-proton-EHISTORY-MUPROD)
add_test(high_low ${CMAKE_SOURCE_DIR}/test/python/high_low.py --input ${CMAKE_SOURCE_DIR}/share/data/DAT000011-proton-EHISTORY-MUPROD)

configure_file(docs/Doxyfile docs/Doxyfile)
add_custom_target(docs doxygen ${CMAKE_BINARY_DIR}/docs/Doxyfile
  COMMAND ${CMAKE_COMMAND} -E env DOXYGEN_XML=${CMAKE_BINARY_DIR}/docs/xml sphinx-build -b html ${CMAKE_SOURCE_DIR}/docs docs/html
  COMMENT building documentation
  )
