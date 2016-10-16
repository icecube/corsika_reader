================
CORSIKA reader
================

C++/Python utility for reading CORSIKA files.

[![Documentation Status](https://readthedocs.org/projects/corsika-reader/badge/?version=latest)](http://corsika-reader.readthedocs.io/en/latest/?badge=latest) [![Build Status](https://travis-ci.org/IceCube-SPNO/corsika_reader.svg?branch=master)](https://travis-ci.org/IceCube-SPNO/corsika_reader)

installation:
=============

```
cmake /path/to/source -DCMAKE_INSTALL_PREFIX=/path/to/install
make
make install
```

The `-DFETCH_CORSIKA_DATA=True` option to cmake fetches some corsika files that are used in tests and examples. They are installed in share/corsika/data. These are downloaded only once.

### Requirements:

0. CMake.
0. Boost development libraries (libboost-...-dev): python, iostreams, test
0. Python (python-devel)

### Some compilation issues:

A problem occurs when using some older versions of boost with cmake-2.8.6-rc2 or later, causing the following error:
```
make[2]: *** No rule to make target `/usr/lib64/lib64/libboost_iostreams-mt.so.5'
```
(note the consecutive lib64/). To avoid this, one can pass the following option to cmake:
```
-DBoost_NO_BOOST_CMAKE=ON
```

This should work, but one can use options like these to make sure Boost and python are found:
```
-DBOOST_ROOT=/opt/boost/1_55_0
-DPYTHON_INCLUDE_DIR=/usr/local/Cellar/python/2.7.11/Frameworks/Python.framework/Versions/2.7/include/python2.7
-DPYTHON_LIBRARY=/usr/local/Cellar/python/2.7.11/Frameworks/Python.framework/Versions/2.7/lib/libpython2.7.dylib
```
or
```
-DBOOST_INCLUDEDIR=$I3_PORTS/include/boost-1.38.0 -DBOOST_LIBRARYDIR=$I3_PORTS/lib/boost-1.38.0
-DPYTHON_INCLUDE_DIR=/cvmfs/icecube.opensciencegrid.org/standard/RHEL_6_x86_64/include/python2.7
-DPYTHON_LIBRARY=/cvmfs/icecube.opensciencegrid.org/standard/RHEL_6_x86_64/lib/libpython2.7.so
```

Examples:
=========

There are a few python examples that get installed in share/corsika/examples. For some plotting examples you need PyRoot, but these will be removed. For others you need matplotlib.
