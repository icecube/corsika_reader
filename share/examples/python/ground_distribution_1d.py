import sys

try:
    import corsika
except:
    from icecube import corsika
import ROOT

if len(sys.argv)>1:
    filename = sys.argv[1]
else:
    filename = corsika.example_data_dir + '/DAT000002-32'

print("opening " + filename)
f = corsika.CorsikaShowerFile(filename)

print("File has %d showers"%f.n_events)

h = ROOT.TH1F()
h.SetBins(100, -6000, 6000)

# corsika distance unit is cm
i = 0
f.find_event(1)
shower = f.current_shower
for p in shower.particles:
    h.Fill(p.x/100)
    i += 1
    if i > 1e4:
        break

h.SetTitle('Air-shower 1d Particle Distribution')
h.GetXaxis().SetTitle('x/m')
h.Draw()
