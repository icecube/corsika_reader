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
f = corsika.CorsikaShowerFile(filename)
f.find_event(1)
events = [f.current_shower]

#for shower in f.events():
for shower in events:
    print "Primary: %s"% shower.primary
    print "Energy: %s"% shower.energy
    print "MuonNumber: %s"% shower.muon_number
    print "Zenith: %s"% shower.zenith
    print "Azimuth: %s"% shower.azimuth
    print "MinRadiusCut: %s"% shower.min_radius_cut
    print "ShowerNumber: %s"% shower.shower_number
    print "EMEnergyCutoff: %s"% shower.em_energy_cutoff
    print "MuonEnergyCutoff: %s"% shower.muon_energy_cutoff

    a=0
    for p in shower.particles:
        if a < 6:
            print p.weight
        a += 1

    print "%s particles"% a
