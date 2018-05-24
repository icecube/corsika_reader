Reading CORSIKA Files in Python
-------------------------------

.. highlightlang:: python



It is quite easy to inspect a CORSIKA file using the low-level
interface. This interface provides an iterator over Blocks, which
correspond to the blocks described in the CORSIKA manual. The Block
class has a method to access the data buffer as a numpy array::

	import corsika
	f = corsika.RawStream(filename)
	block = corsika.Block()
	f.get_next_block(block)
	print block.ID
	f.get_next_block(block)
	print block.ID
	print block.data

The particles in a low-level file (RawCorsikaFile) can be accessed
using the particle iterator. In this case, the iteration over blocks
is done behind the scenes::

	particles = [(p.x, p.y, p.t_or_z) for p in f.particles()]

Inspecting a CORSIKA file using the high-level interface in python::

	import corsika
	f = corsika.ShowerFile(filename)
	particles = f.current_shower.particles
	print len([p for p in particles]), 'particles'
	particles.rewind()

More examples can be found in the examples directory.

