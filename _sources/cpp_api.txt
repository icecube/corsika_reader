
.. index:: C++ Interface
.. _cpp_CorsikaReader:

C++ Interface Reference
=======================

High-level Interface
--------------------

.. doxygenclass:: corsika::ShowerFile
   :members:

.. doxygenclass:: corsika::Particle
   :members:

Low-level Interface
-------------------

.. doxygenclass:: corsika::RawStream
   :members: Create, SeekTo, IsSeekable, IsThinned, GetNextBlock, GetNextPosition
.. doxygenclass:: corsika::Block
   :members:

