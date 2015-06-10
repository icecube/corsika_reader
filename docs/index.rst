
.. index:: CORSIKA Reader
.. _CorsikaReader:

CORSIKA Reader
==============

The CORSIKA reader provides a (more or less) generic interface to
`CORSIKA`_ files. CORSIKA files can be accessed using either a C++ or
a Python interface. The reader can also handle CORSIKA files
compressed with gzip or bzip2.

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

.. _`CORSIKA`: http://www.ikp.kit.edu/corsika/

Reading CORSIKA Files in Python
-------------------------------

.. highlightlang:: python



It is quite easy to inspect a CORSIKA file using the low-level
interface. This interface provides an iterator over Blocks, which
correspond to the blocks described in the CORSIKA manual. The Block
class has a method to access the data buffer as a numpy array::

	import corsika
	f = corsika.RawCorsikaFile()
	block = corsika.Block()
	f.GetNextBlock(block)
	print block.ID()
	f.GetNextBlock(block)
	print block.ID()
	print block.data

The particles in a low-level file (RawCorsikaFile) can be accessed
using the particle iterator. In this case, the iteration over blocks
is done behind the scenes::

	particles = [(p.fX, p.fY, p.fZ) for p in f.particles()]

Inspecting a CORSIKA file using the high-level interface in python::

	import corsika
	f = corsika.CorsikaShowerFile()
	shower = f.GetEvent(1)
	particles = shower.particles()
	print len([p for p in particles]), 'particles'
	particles.rewind()

More examples can be found in the examples directory.

Design Choices
--------------

Apart from the decision to split the library into high- and low-level
interfaces, there are a few choices that should be documented and
reconsidered if necessary. A guiding principle has been to write small
classes with a well defined purpose so they can be unit-tested.

* Iterators
* Underlying stream state
* Random access
* Block as a union

Low-level Python Interface
--------------------------

.. autoclass:: icecube.corsika.RawFile
   :members: Open, Close, GetNextBlock, particles, IsOpen, IsValid
.. autoclass:: icecube.corsika.Block
   :members: ID, data, IsRunHeader, IsRunTrailer, IsEventHeader, IsEventTrailer, IsLongitudinal
