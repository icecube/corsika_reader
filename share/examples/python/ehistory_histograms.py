import numpy
import matplotlib.pyplot as plt
plt.ion()

try:
    import icecube
    within_icecube = True
except:
    within_icecube = False

if within_icecube: from icecube import corsika
else: import corsika

import sys

if len(sys.argv)>1:
    filename = sys.argv[1]
else:
    filename = corsika.example_data_dir + '/DAT000011-proton-EHISTORY-MUPROD'

f = corsika.ShowerFile(filename)
f.find_event(1)

shower = f.current_shower

ptype = numpy.dtype([('code',int), ('pdg',int), ('px',float), ('py',float), ('pz',float), ('x',float), ('y',float), ('z_or_t', float), ('h',int), ('energy',float)])

def particle2dtype(particle, row):
    row['code'] = particle.corsika_code
    row['pdg'] = particle.pdg_code
    row['px'] = particle.px
    row['py'] = particle.px
    row['pz'] = particle.px
    row['x'] = particle.x
    row['y'] = particle.y
    row['z_or_t'] = particle.t_or_z
    row['h'] = particle.hadronic_generation
    row['energy'] = particle.kinetic_energy



particle_it = shower.particles
l = len([p for p in particle_it if p.has_parent])
daughters = numpy.zeros(l, dtype=ptype)
parents = numpy.zeros(l, dtype=ptype)
grand_parents = numpy.zeros(l, dtype=ptype)

particle_it.rewind()
i = 0
for p in particle_it:
    if not p.has_parent: continue
    particle2dtype(p, daughters[i])
    particle2dtype(p.parent, parents[i])
    particle2dtype(p.grand_parent, grand_parents[i])
    i += 1


x_bins = sorted(set(daughters['code']))
x_names = [corsika.ParticleList().name_from_corsika(c) for c in x_bins]
y_bins = sorted(set(parents['code']))
y_names = [corsika.ParticleList().name_from_corsika(c) for c in y_bins]
yy_bins = sorted(set(grand_parents['code']))
yy_names = [corsika.ParticleList().name_from_corsika(c) for c in yy_bins]

contents = numpy.zeros((len(x_bins), len(y_bins)), dtype=int)
for i,x in enumerate(x_bins):
    for j,y in enumerate(y_bins):
        contents[i,j] = numpy.sum((daughters['code']==x)*(parents['code']==y))

figure = plt.figure('parent_daughter')
plt.imshow(contents.T, interpolation='nearest')
plt.xticks(range(len(x_names)), x_names, rotation=30)
plt.yticks(range(len(y_names)), y_names)
plt.colorbar()

grand_contents = numpy.zeros((len(x_bins), len(yy_bins)), dtype=int)
for i,x in enumerate(x_bins):
    for j,y in enumerate(yy_bins):
        grand_contents[i,j] = numpy.sum((daughters['code']==x)*(grand_parents['code']==y))

figure = plt.figure('grand_parent_daughter')
plt.imshow(grand_contents.T, interpolation='nearest')
plt.xticks(range(len(x_names)), x_names, rotation=30)
plt.yticks(range(len(yy_names)), yy_names)
plt.colorbar()
