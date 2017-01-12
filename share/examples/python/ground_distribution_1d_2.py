import sys

try:
    import icecube
    within_icecube = True
except:
    within_icecube = False

if within_icecube: from icecube import corsika
else: import corsika

import ROOT
import numpy

if len(sys.argv)>1:
    filename = sys.argv[1]
else:
    filename = corsika.example_data_dir + '/DAT000002-32'

print("opening " + filename)
f = corsika.CorsikaShowerFile(filename)

print("File has %d showers"%f.n_events)

h = ROOT.TH1F()
n_bins = 50
bins = numpy.logspace(0, 3, n_bins+1)
h.SetBins(n_bins, bins)

# corsika distance unit is cm
i = 0
for shower in f.events():
    for p in shower.particles:
        h.Fill(p.x/100)
        i += 1
        if i > 1e4:
            break

h.SetTitle('Air-shower 1d Particle Distribution')
h.GetXaxis().SetTitle('x/m')
h.Draw()
ROOT.gPad.SetLogx()
