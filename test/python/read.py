
import unittest
import os, sys

try:
    import icecube
    within_icecube = True
except:
    within_icecube = False

if within_icecube: from icecube import corsika
else: import corsika


class ReadTest(unittest.TestCase):
    def test(self):
        filename = corsika.example_data_dir + '/DAT000002-32'
        assert os.path.exists(filename)
        f = corsika.CorsikaShowerFile(filename)
        assert f.find_event(1) == corsika.Status.eSuccess

        shower = f.current_shower

        assert shower.primary == 2212
        assert shower.energy == 100000.0
        assert shower.muon_number == 1335.0
        self.assertAlmostEqual(shower.zenith, 0.3490658, 6)
        self.assertAlmostEqual(shower.azimuth, 3.0609822, 6)
        assert shower.min_radius_cut == 0.
        assert shower.shower_number == 1
        self.assertAlmostEqual(shower.em_energy_cutoff, 0.003)
        self.assertAlmostEqual(shower.muon_energy_cutoff, 0.3)

        a=0
        for p in shower.particles:
            if a == 0:
                assert p.weight == 1
            a += 1
        assert a == 181992
        assert f.find_event(2) == corsika.Status.eFail

if __name__ == '__main__':
    unittest.main()
