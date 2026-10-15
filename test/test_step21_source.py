# My-NetHack modification notice, added 2026-10-15.
# Local project integration/modifications; original authorship is retained.
# Recorded project change dates (Git author dates): 2026-10-10.
# See doc/license-compliance.md and doc/my-nethack-modifications.json.
# Existing copyright, attribution, license and warranty notices still apply.
"""Structural safeguards, complemented by native Step 21 gameplay fixtures."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]

class Step21(unittest.TestCase):
    def test_epoch_and_authoritative_state(self):
        self.assertRegex((ROOT/'include/patchlevel.h').read_text(), r'#define EDITLEVEL 17\b')
        obj = (ROOT/'include/obj.h').read_text()
        self.assertIn('oartifact_state', obj)
        self.assertNotIn('artifact_cooldown', obj)

    def test_single_catalog_entry(self):
        text = (ROOT/'include/artilist.h').read_text()
        self.assertEqual(text.count('A("The Silver Key"'), 1)
        added = text.split('/* Step 21:', 1)[1].split('#if !defined(ARTI_ENUM)', 1)[0]
        self.assertEqual(len(re.findall(r'\bA\("', added)), 22)
        self.assertNotIn('SPFX_NOGEN', added)

    def test_mirror_shared_native_handler(self):
        self.assertIn('return use_mirror(obj);', (ROOT/'src/artifact.c').read_text())
        self.assertIn('res = use_mirror(obj);', (ROOT/'src/apply.c').read_text())

if __name__ == '__main__':
    unittest.main()
