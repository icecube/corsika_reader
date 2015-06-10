#!/usr/bin/env python
import sys

try:
    import corsika
except:
    from icecube import corsika
from ROOT import *

if len(sys.argv)>1:
    filename = sys.argv[1]
else:
    filename = corsika.example_data_dir + '/DAT000002-32'

f = corsika.CorsikaShowerFile(filename)

h = TH2F('ground_particles', 'Ground Particles', 100, -5000, 5000, 100, -5000, 5000)
h.GetXaxis().SetTitle('X/m')
h.GetYaxis().SetTitle('Y/m')

f.find_event(1)
for particle in f.current_shower.particles:
    c = h.Fill(particle.x/100., particle.y/100.)

h.Draw('surf1')
gPad.SetLogz()
