
.. index:: C++ Interface
.. _cpp_CorsikaReader:

C++ Interface Reference
=======================

High-level Interface
--------------------

.. doxygenclass:: corsika::CorsikaShowerFile
   :members:

.. doxygenclass:: corsika::CorsikaParticle
   :members:

Low-level Interface
-------------------

.. doxygenclass:: corsika::Corsika::RawFile
   :members: open, close, get_next_block, particles, is_open, is_valid
.. doxygenclass:: corsika::Corsika::Block
   :members:

