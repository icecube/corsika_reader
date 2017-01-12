#!/usr/bin/env python

import sys
try:
    import icecube
    within_icecube = True
except:
    within_icecube = False

if within_icecube: from icecube import corsika
else: import corsika


if len(sys.argv)>1:
    filename = sys.argv[1]
else:
    filename = corsika.example_data_dir + '/DAT000002-32'
raw = corsika.RawStream(filename)

block = corsika.Block()


raw.get_next_block(block)
print block.ID
raw.get_next_block(block)
print block.ID

particles = raw.particles()
p = particles.next()

n_blocks = 2
while raw.get_next_block(block):
    n_blocks += 1

print n_blocks, 'blocks'
assert n_blocks == 4724
#print "%s particles"% len([p for p in particles if not int(p.fDescription/1000) in [0, 75, 76, 85, 86, 95, 96]])
