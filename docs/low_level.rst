
.. index:: Low-level CORSIKA Reader
.. _low_level_CorsikaReader:

Low-level Python Interface
--------------------------

.. autoclass:: corsika.RawFile
   :members: open, close, get_next_block, particles, is_open, is_valid
.. autoclass:: corsika.Block
   :members:


Low-level C++ Interface
--------------------------

.. doxygenclass:: corsika::Corsika::RawFile
   :members: open, close, get_next_block, particles, is_open, is_valid
.. doxygenclass:: corsika::Corsika::Block
   :members:
