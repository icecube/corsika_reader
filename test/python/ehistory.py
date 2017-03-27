#!/usr/bin/env python

import unittest
import numpy
try:
    import icecube
    with_icecube = True
except:
    with_icecube = False

if with_icecube: from icecube import corsika
else: import corsika

files = []

class EHistoryTest(unittest.TestCase):
    def test(self):
        global files
        if not files: files = [corsika.example_data_dir + '/DAT000011-proton-EHISTORY-MUPROD']
        for f in files:
            if not os.path.exists(f): print f, 'does not exist'
            assert os.path.exists(f)

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


if __name__ == '__main__':
    # unittest uses sys.argv, so in order to pass arguments we read them first and set sys.argv so it is k for unittest
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--input', help='Input file', action='append')
    parser.add_argument('unittest_args', nargs='*')

    args = parser.parse_args()
    if args.input: files = args.input

    sys.argv[1:] = args.unittest_args
    unittest.main()
