
.. index:: Python Interface
.. _python_CorsikaReader:

Python Interface
================

The python interface currently does not show in readthedocs. It requires that the C++ library is built so sphinx can inspect it.

High-level Interface
--------------------

.. autoclass:: corsika.CorsikaShowerFile
   :members:

.. autoclass:: corsika.CorsikaParticle
   :members:

Low-level Interface
-------------------

.. autoclass:: corsika.RawFile
   :members: open, close, get_next_block, particles, is_open, is_valid
.. autoclass:: corsika.Block
   :members:


