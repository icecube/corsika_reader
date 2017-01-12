#!/usr/bin/env python

import numpy
try:
    import icecube
    within_icecube = True
except:
    within_icecube = False

if within_icecube: from icecube import corsika
else: import corsika

import sys

if len(sys.argv)>1:
    files = sys.argv[1:]
else:
    files = [corsika.example_data_dir + '/DAT000011-proton-EHISTORY-MUPROD']

for f in files:
    print '\n', f
    raw = corsika.RawStream(f)
    block = corsika.Block()
    raw.get_next_block(block)
    raw.get_next_block(block)
    particles = raw.particles()
    descriptions = numpy.array([int(p.description/1000) for p in particles if int(p.description/1000) != 0])

    exclude = []
    ehistory = (not numpy.all(descriptions > 0))
    if ehistory:
        print 'File with EHISTORY option'
    if 75 in descriptions or 76 in descriptions:
        print 'File with MUADDI option'
    if 95 in descriptions or 96 in descriptions:
        print 'File with MUPROD option'

    muaddi = numpy.where((descriptions==75)+(descriptions==76))[0]
    if sum(muaddi):
        print 'Muon additional information:', set(descriptions[muaddi])
        exclude += [muaddi]
        if ehistory:
            print '               grandparents:', set(descriptions[muaddi+1])
            print '                    parents:', set(descriptions[muaddi+2])
            exclude += [muaddi+1, muaddi+2]
            assert sorted(set(descriptions[muaddi+3])) == [5, 6]
        else:
            assert sorted(set(descriptions[muaddi+1])) == [5, 6]

    muprod = numpy.where((descriptions==85)+(descriptions==86))[0]
    if sum(muprod):
        print 'Decaying muons:             ', set(descriptions[muprod])
        exclude += [muprod]
        if ehistory:
            print '               grandparents:', set(descriptions[muprod+1])
            print '                    parents:', set(descriptions[muprod+2])
            exclude += [muprod+1, muprod+2]
            assert sorted(set(descriptions[muaddi+3])) == [5, 6]
        else:
            assert sorted(set(descriptions[muaddi+1])) == [5, 6]


    m = numpy.ones_like(descriptions, dtype=bool)
    m[numpy.concatenate(exclude)] = False
    print 'the rest:                   ', set(descriptions[m])
