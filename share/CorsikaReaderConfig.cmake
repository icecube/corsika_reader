
find_package (PythonLibs)
find_package (Boost REQUIRED COMPONENTS iostreams python unit_test_framework)

# the library
find_library (CORSIKA_READER_LIBRARY CorsikaReader PATHS @CMAKE_INSTALL_PREFIX@/lib NO_DEFAULT_PATH)
set (CORSIKA_READER_INCLUDE_DIR @CMAKE_INSTALL_PREFIX@/include)

# all dependencies included
set (CorsikaReader_INCLUDE_DIRS ${CORSIKA_READER_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
set (CorsikaReader_LIBRARIES ${CORSIKA_READER_LIBRARY} ${CORSIKA_READER_LIBRARY} ${PYTHON_LIBRARIES})

# the only exported library
add_library(CorsikaReader SHARED IMPORTED)
set_property(TARGET CorsikaReader PROPERTY IMPORTED_LOCATION ${CORSIKA_READER_LIBRARY})
set_property(TARGET CorsikaReader PROPERTY IMPORTED_IMPLIB ${CORSIKA_READER_LIBRARY})
