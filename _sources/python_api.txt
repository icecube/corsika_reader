
.. index:: Python Interface
.. _python_CorsikaReader:

Python Interface Reference
==========================

The python interface currently does not show in readthedocs. It requires that the C++ library is built so sphinx can inspect it.

High-level Interface
--------------------

.. autoclass:: corsika.ShowerFile
   :members:

.. autoclass:: corsika.Particle
   :members:

Low-level Interface
-------------------

.. autoclass:: corsika.RawStream
   :members: close, get_next_block, particles, is_open
.. autoclass:: corsika.Block
   :members:


