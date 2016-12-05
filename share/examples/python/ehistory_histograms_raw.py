import numpy
import matplotlib.pyplot as plt
plt.ion()
import sys
try:
    import corsika
except:
    from icecube import corsika

if len(sys.argv)>1:
    filename = sys.argv[1]
else:
    filename = corsika.example_data_dir + '/DAT000011-proton-EHISTORY-MUPROD'

raw = corsika.RawStream(filename)
block = corsika.Block()
raw.get_next_block(block) # RUNH
raw.get_next_block(block) # EVTH
raw_particle_it = raw.particles()

# Fill a numpy array with particles
raw_ptype = numpy.dtype([('code',int), ('h',int), ('level',int), ('px',float), ('py',float), ('pz',float), ('x',float), ('y',float), ('z_or_t', float)])

def raw_particle2dtype(particle, row):
    row['code'] = abs(int(particle.description/1000))
    row['h'] = abs(int((particle.description%1000)/10))
    row['level'] = abs(int(particle.description%10))
    row['px'] = particle.px
    row['py'] = particle.py
    row['pz'] = particle.pz
    row['x'] = particle.x
    row['y'] = particle.y
    row['z_or_t'] = particle.t_or_z


raw_particles = numpy.zeros(len([p for p in raw_particle_it]), dtype=raw_ptype)

raw_particle_it.rewind()
i = 0
for p in raw_particle_it:
    raw_particle2dtype(p, raw_particles[i])
    i += 1

# find the muons at observation level 1 and their parents
m = ((raw_particles['code']==6)+(raw_particles['code']==5)+(raw_particles['code']==95)+(raw_particles['code']==96))*(raw_particles['level']==1)
daughters_raw = raw_particles[numpy.where(m)[0]]
parents_raw = raw_particles[numpy.where(m)[0] - 2]
grand_parents_raw = raw_particles[numpy.where(m)[0] - 1]

# count them and plot
x_bins = sorted(set(daughters_raw['code']))
x_names = [corsika.ParticleList.name_from_corsika(c) for c in x_bins]
y_bins = sorted(set(parents_raw['code']))
y_names = [corsika.ParticleList.name_from_corsika(c) for c in y_bins]
yy_bins = sorted(set(grand_parents_raw['code']))
yy_names = [corsika.ParticleList.name_from_corsika(c) for c in yy_bins]

contents_raw = numpy.zeros((len(x_bins), len(y_bins)), dtype=int)
for i,x in enumerate(x_bins):
    for j,y in enumerate(y_bins):
        contents_raw[i,j] = numpy.sum((daughters_raw['code']==x)*(parents_raw['code']==y))

figure = plt.figure('parent_daughter_raw')
plt.imshow(contents_raw.T, interpolation='nearest')
plt.xticks(range(len(x_names)), x_names, rotation=30)
plt.yticks(range(len(y_names)), y_names)
plt.colorbar()

grand_contents_raw = numpy.zeros((len(x_bins), len(yy_bins)), dtype=int)
for i,x in enumerate(x_bins):
    for j,y in enumerate(yy_bins):
        grand_contents_raw[i,j] = numpy.sum((daughters_raw['code']==x)*(grand_parents_raw['code']==y))

figure = plt.figure('grand_parent_daughter_raw')
plt.imshow(grand_contents_raw.T, interpolation='nearest')
plt.xticks(range(len(x_names)), x_names, rotation=30)
plt.yticks(range(len(yy_names)), yy_names)
plt.colorbar()
