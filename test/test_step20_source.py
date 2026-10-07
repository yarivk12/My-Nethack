"""Step 20 contracts which are not exercised by ordinary catalogue generation."""
from pathlib import Path
import unittest, re
R = Path(__file__).resolve().parents[1]
class Step20(unittest.TestCase):
    def test_epoch(self):
        self.assertRegex((R/'include/patchlevel.h').read_text(), r'#define EDITLEVEL 16\b')
    def test_roster_and_generation(self):
        source = (R/'src/makemon.c').read_text()
        for name, depth in [('VAMPIRE_MAGE',50),('DEEPEST_ONE',50),('DRIDER',55),
                ('ASTRAL_DEVA',60),('SHOGGOTH',60),('DEATH_KNIGHT',65),
                ('HOUND_OF_TINDALOS',70),('PLANETAR',70),('VORPAL_JABBERWOCK',80),
                ('NEOTHELID',80),('GUG',85),('GIANT_SHOGGOTH',85),('VOID_DRAGON',90),
                ('PRIESTESS_OF_GHAUNADAUR',95),('ALHOON',95),('SOLAR',95),
                ('JUGGERNAUT',100),('ELDER_BRAIN',130)]:
            self.assertRegex(source, rf'case PM_{name}: return {depth};')
    def test_no_new_attack_enums(self):
        source = (R/'include/monattk.h').read_text()
        self.assertNotRegex(source, r'#define (?:AT|AD)_WEB')
if __name__ == '__main__': unittest.main()
