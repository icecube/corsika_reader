
import unittest
import sys, os
try:
    import corsika
except:
    from icecube import corsika

class RawTest(unittest.TestCase):
    def test(self):


        filename = corsika.example_data_dir + '/DAT000002-32'
        raw = corsika.RawStream(filename)

        block = corsika.Block()


        raw.get_next_block(block)
        assert block.ID == 'RUNH'
        raw.get_next_block(block)
        assert block.ID == 'EVTH'

        particles = raw.particles()
        p = particles.next()

        n_blocks = 2
        while raw.get_next_block(block):
            n_blocks += 1

        assert n_blocks == 4724

if __name__ == '__main__':
    unittest.main()

