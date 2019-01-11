#!/usr/bin/env python

import unittest
import numpy
import sys, os

sys.path.append('')
try:
    import icecube
    with_icecube = True
except:
    with_icecube = False

if with_icecube: from icecube import corsika
else: import corsika


ptype = numpy.dtype([('code',int), ('pdg',int), ('px',float), ('py',float), ('pz',float), ('x',float), ('y',float), ('z_or_t', float), ('h',int), ('energy',float)])
def particle2dtype(particle, row):
    row['code'] = particle.corsika_code
    row['pdg'] = particle.pdg_code
    row['px'] = particle.px
    row['py'] = particle.py
    row['pz'] = particle.pz
    row['x'] = particle.x
    row['y'] = particle.y
    row['z_or_t'] = particle.t_or_z
    row['h'] = particle.hadronic_generation
    row['energy'] = particle.kinetic_energy





raw_ptype = numpy.dtype([('description',int), ('code',int), ('pdg',int), ('h',int), ('level',int), ('px',float), ('py',float), ('pz',float), ('x',float), ('y',float), ('z_or_t', float), ('history',bool)])
def raw_particle2dtype(particle, row):
    row['history'] == particle.description<0
    row['description'] = int(particle.description)
    row['code'] = abs(int(particle.description/1000))
    row['pdg'] = corsika.ParticleList.corsika_to_pdg(abs(int(particle.description/1000)))
    row['h'] = abs(int((particle.description%1000)/10))
    row['level'] = abs(int(particle.description%10))
    row['px'] = particle.px
    row['py'] = particle.py
    row['pz'] = particle.pz
    row['x'] = particle.x
    row['y'] = particle.y
    row['z_or_t'] = particle.t_or_z

filename = None

class HighLowTest(unittest.TestCase):
    def test(self):
        global filename
        if filename is None: filename = corsika.example_data_dir + '/DAT000011-proton-EHISTORY-MUPROD'
        assert os.path.exists(filename)

        #print 'opening', filename
        f = corsika.ShowerFile(filename)
        f.find_event(1)
        shower = f.current_shower
        particle_it = shower.particles


        raw = corsika.RawStream(filename)
        block = corsika.Block()
        raw.get_next_block(block) # RUNH
        raw.get_next_block(block) # EVTH
        raw_particle_it = raw.particles()

        raw_particles = numpy.zeros(len([p for p in raw_particle_it]), dtype=raw_ptype)
        raw_particle_it.rewind()
        i = 0
        for p in raw_particle_it:
            raw_particle2dtype(p, raw_particles[i])
            i += 1


        particle_it = shower.particles
        l = len([p for p in particle_it])
        particles = numpy.zeros(l, dtype=ptype)
        parents = numpy.zeros(l, dtype=ptype)
        grand_parents = numpy.zeros(l, dtype=ptype)
        parents['code'] = -99999999999
        grand_parents['code'] = -99999999999
        particle_it.rewind()
        i = 0
        for p in particle_it:
            particle2dtype(p, particles[i])
            if p.has_parent:
                particle2dtype(p.parent, parents[i])
                particle2dtype(p.grand_parent, grand_parents[i])
            i += 1

        parent_mask = parents['code'] != -99999999999
        grandparent_mask = grand_parents['code'] != -99999999999

        muon_mask = (particles['code']==6)+(particles['code']==5)+(particles['code']==95)+(particles['code']==96)
        electron_mask = (particles['code']==3)+(particles['code']==2)
        gamma_mask = (particles['code']==1)

        raw_muon_mask = (raw_particles['code']==6)+(raw_particles['code']==5)+(raw_particles['code']==95)+(raw_particles['code']==96)
        raw_electron_mask = (raw_particles['code']==3)+(raw_particles['code']==2)
        raw_gamma_mask = (raw_particles['code']==1)

        # the following raw entries are filtered by the high-level interface:
        raw_invalid_mask = raw_particles['description']==0   # these are trailing particles, between the last particle and the end of the block
        raw_history_mask = raw_particles['description']<0    # these are EHISTORY particles, which are accessed as members of high-level particles
        raw_undefined_mask = raw_particles['pdg']==0         # these entries have no entry in the particle table (very rare particles)

        muons = particles[muon_mask]
        raw_muons = raw_particles[raw_muon_mask*~raw_invalid_mask*~raw_history_mask*~raw_undefined_mask*(raw_particles['level']==1)]

        def test(condition, message):
            if not condition: print 'FAIL:', message
            assert test
        def test_equal(left, right, message):
            if not left==right: print 'FAIL:', message, ":", left, "!=", right
            assert left == right

        test_equal(len(raw_particles), 294645, "total number of raw particles")

        test_equal(len(particles), 254188, "total number of particles")

        test_equal(len(particles), len(raw_particles[~raw_invalid_mask*~raw_history_mask*~raw_undefined_mask*(raw_particles['level']==1)]),
                   "discrepancy in number of particles")

        test_equal(len(raw_particles[raw_muon_mask*~raw_invalid_mask*~raw_history_mask*~raw_undefined_mask*(raw_particles['level']==1)]), len(particles[muon_mask]),
                   "discrepancy in number of muons")

        test_equal(len(raw_particles[raw_electron_mask*~raw_invalid_mask*~raw_history_mask*~raw_undefined_mask*(raw_particles['level']==1)]), len(particles[electron_mask]),
                   "discrepancy in number of electrons")

        test_equal(len(raw_particles[raw_gamma_mask*~raw_invalid_mask*~raw_history_mask*~raw_undefined_mask*(raw_particles['level']==1)]), len(particles[gamma_mask]),
                   "discrepancy in number of gammas")

        test_equal(len(particles[parent_mask*muon_mask]), len(particles[muon_mask]), "discrepancy in number of muons and muon parents")

        test(numpy.all(raw_muons['x']==muons['x']), 'x differs')
        test(numpy.all(raw_muons['y']==muons['y']), 'y differs')
        test(numpy.all(raw_muons['px']==muons['px']), 'px differs')
        test(numpy.all(raw_muons['py']==muons['py']), 'py differs')

        t_offset = numpy.mean(raw_muons['z_or_t']-muons['z_or_t']) # particle times might be shifted
        test(numpy.all(numpy.abs(raw_muons['z_or_t']-muons['z_or_t'] - t_offset) < 1e-1), 'T or z differs')


if __name__ == '__main__':
    # unittest uses sys.argv, so in order to pass arguments we read them first and set sys.argv so it is k for unittest
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--input', help='Input file')
    parser.add_argument('unittest_args', nargs='*')

    args = parser.parse_args()
    if args.input: filename = args.input

    sys.argv[1:] = args.unittest_args
    unittest.main()
