#!/usr/bin/env python

import sys
try:
    import icecube
    within_icecube = True
except:
    within_icecube = False

if within_icecube: from icecube import corsika
else: import corsika

import numpy

if len(sys.argv)>1:
    filename = sys.argv[1]
else:
    filename = corsika.example_data_dir + '/DAT000002-32'
raw = corsika.RawStream(filename)

block = corsika.Block()


raw.get_next_block(block)
print block.ID, 'at', raw.get_next_position()-1


while not block.is_run_trailer:
    raw.get_next_block(block)
    if block.is_control:
        print block.ID, 'at', raw.get_next_position()-1


raw.seek_to(2) # right after the EVTH, as you can see in the previous output
raw.get_next_block(block)

# if in this day and age you do not have a wider monitor...
numpy.set_printoptions(linewidth=150, precision=4)
print 'first particle block after first EVTH:\n', block.data.reshape((-1,7))
