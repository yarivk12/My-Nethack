"""Step 19 catalogue contracts, complemented by native runtime fixtures."""
from pathlib import Path
import re
import unittest
from step20_source_projection import project as project_step20

ROOT = Path(__file__).resolve().parents[1]

class Catalogue(unittest.TestCase):
    def test_ordinary_equipment(self):
        text = (ROOT / 'include/objects.h').read_text()
        for name in ('gain intelligence', 'gain wisdom', 'carrying',
                     'amulet of power', 'passwall', 'repair armor',
                     'sacrificial knife'):
            self.assertIn('"' + name + '"', text)

    def test_amulet_probabilities(self):
        text = (ROOT / 'include/objects.h').read_text()
        amulets = dict(re.findall(r'AMULET\("([^"]+)",\s*"[^"]+",\s*\w+,\s*(\d+),', text))
        self.assertEqual(sum(map(int, amulets.values())), 1000)
        self.assertEqual(amulets.get('amulet of power'), '60')
        for name in ('change', 'restful sleep', 'strangulation'):
            self.assertEqual(amulets['amulet of ' + name], '95')

    def test_spell_probability_funding(self):
        text = (ROOT / 'include/objects.h').read_text()
        spells = {name: (school, int(prob), int(level)) for name, school, prob, level
                  in re.findall(r'SPELL\("([^"]+)",\s*"[^"]+",\s*(P_\w+),\s*(\d+),\s*\d+,\s*(\d+),', text)}
        self.assertEqual(spells['passwall'], ('P_ESCAPE_SPELL', 20, 6))
        self.assertEqual(spells['repair armor'], ('P_MATTER_SPELL', 20, 3))
        for name, prob in [('knock', 15), ('wizard lock', 15),
                           ('confuse monster', 39), ('slow monster', 20)]:
            self.assertEqual(spells[name][1], prob)

    def test_explicit_custom_objects(self):
        origin = (ROOT / 'src/objects.c').read_text()
        names = ['RIN_GAIN_INTELLIGENCE', 'RIN_GAIN_WISDOM', 'RIN_CARRYING',
                 'AMULET_OF_POWER', 'SPE_PASSWALL', 'SPE_REPAIR_ARMOR',
                 'SACRIFICIAL_KNIFE', 'MUMMIFIED_HAND']
        for dragon in ['SHIMMERING', 'DEEP', 'RAZOR', 'FILTH', 'SHADOW', 'CELESTIAL']:
            names.extend([dragon + '_DRAGON_SCALES', dragon + '_DRAGON_SCALE_MAIL'])
        self.assertEqual(len(names), 20)
        for name in names:
            self.assertIn('case ' + name + ':', origin)

    def test_catalogue_version(self):
        patchlevel = (ROOT / 'include/patchlevel.h').read_text()
        # Keep Step 19's exact epoch after reversing only the reviewed successor
        # delta. The current epoch is separately pinned by Step 20's source gate.
        patchlevel = project_step20('include/patchlevel.h', patchlevel)
        self.assertEqual(re.findall(r'^#define EDITLEVEL (\d+)$', patchlevel, re.M), ['15'])

if __name__ == '__main__':
    unittest.main()
