import sys

try:
    import icecube
    within_icecube = True
except:
    within_icecube = False

if within_icecube: from icecube import corsika
else: import corsika

import ROOT

if len(sys.argv)>1:
    filename = sys.argv[1]
else:
    filename = corsika.example_data_dir + '/DAT000002-32'

print("opening " + filename)
f = corsika.ShowerFile(filename)

print("File has %d showers"%f.n_events)

h = ROOT.TH2F()
h.SetBins(100, -6000, 6000, 100, -6000, 6000)

# corsika distance unit is cm
i = 0
for shower in f.events():
    for p in shower.particles:
        h.Fill(p.x/100, p.y/100)
        i += 1
        if i > 1e4:
            break

h.SetTitle('Air-shower 2d Particle Distribution')
h.GetXaxis().SetTitle('x/m')
h.GetYaxis().SetTitle('x/m')
h.Draw('colz')
