================
CORSIKA reader
================
[![Build Status](https://travis-ci.org/IceCube-SPNO/corsika_reader.svg?branch=master)](https://travis-ci.org/IceCube-SPNO/corsika_reader)

C++/Python utility for reading CORSIKA files.

Requirements:
=============

0. CMake.
0. Boost development libraries (libboost-...-dev): python, iostreams, test

installation:
=============

```
cmake /path/to/source -DCMAKE_INSTALL_PREFIX=/path/to/install -DFETCH_CORSIKA_DATA=True
make
make install
```

### Compile issues:

A problem occurs when using some older versions of boost with cmake-2.8.6-rc2 or later causing the following error:
  make[2]: *** No rule to make target `/usr/lib64/lib64/libboost_iostreams-mt.so.5'
(note the consecutive lib64/). To avoid this, one can pass the following option to cmake:
  -DBoost_NO_BOOST_CMAKE=ON

One can use options like these to make sure Boost and python are found:
 -DBOOST_ROOT=/opt/boost/1_55_0
 -DPYTHON_INCLUDE_DIR=/Users/javierg/Python/Environments/maverick/include/python2.7
 -DPYTHON_LIBRARY=/usr/local/Cellar/python/2.7.6/Frameworks/Python.framework/Versions/2.7/lib/libpython2.7.dylib
or
 -DPYTHON_INCLUDE_DIR=/cvmfs/icecube.opensciencegrid.org/standard/RHEL_6_x86_64/include/python2.7 -DPYTHON_LIBRARY=/cvmfs/icecube.opensciencegrid.org/standard/RHEL_6_x86_64/lib/libpython2.7.so
 -DBOOST_INCLUDEDIR=$I3_PORTS/include/boost-1.38.0 -DBOOST_LIBRARYDIR=$I3_PORTS/lib/boost-1.38.0

Examples:
=========

There are a few python examples that get installed in share/corsika/examples. For some plotting examples you need PyRoot, but these will be removed. For others you need matplotlib.
