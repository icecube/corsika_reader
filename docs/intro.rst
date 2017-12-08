
Introduction
------------

The CORSIKA reader provides a (more or less) generic interface to
`CORSIKA`_ files. CORSIKA files can be accessed using either a C++ or
a Python interface. The reader can also handle CORSIKA files
compressed with gzip or bzip2. The source can be found in `github`_.

The CORSIKA reader library is composed of two major parts, a low- and
a high-level interface. The low-level interface is the more general
one, and consists of the minimum required code to iterate over the blocks in
a file, and to cast these blocks into C++ structures:

* Block
* RawCorsikaFile
* RawParticleIterator

The high-level interface has a similar structure, but these classes
have methods that help in dealing with some of CORSIKA's
idiosyncrasies. For example, dealing with particle codes (CORSIKA Vs
PDG), coordinate systems (flat versus curved atmosphere), sorting out
special particles (muon additional information, EHISTORY), whatever
else:

* CorsikaParticle
* ShowerParticleList
* CorsikaShowerFile
* CorsikaShowerFileParticleIterator
* GaisserHillasParameter
* CorsikaLongProfile
* ParticleList

The rest of the classes are some utilities:

* CorsikaIOException
* Units
* Physical and mathematical constants
* CorsikaUtilities
* Index

A guiding principle has been to write small
classes with a well defined purpose so they can be unit-tested.

* Iterators
* Underlying stream state
* Random access
* Block as a union


.. _`CORSIKA`: http://www.ikp.kit.edu/corsika/
.. _`github`: https://github.com/IceCube-SPNO/corsika_reader
