# My-NetHack modification notice, added 2026-10-15.
# Local project integration/modifications; original authorship is retained.
# Recorded project change dates (Git author dates): 2026-09-19, 2026-09-24.
# See doc/license-compliance.md and doc/my-nethack-modifications.json.
# Existing copyright, attribution, license and warranty notices still apply.
"""Reverse only frozen Step 15C production hunks for earlier identity gates.

The Step 15B/audit manifests and original baselines remain immutable.
test_step15c_source.py rejects mutations inside and outside these hunks.
"""
from step15d_source_projection import project as step15d_project
import json
from pathlib import Path

CHANGES = json.loads(Path(__file__).with_name(
    'step15c_historical_changes.json').read_text(encoding='utf8'))


def project(path, text):
    text = step15d_project(path, text)
    for hunk in CHANGES.get(path, []):
        if hunk['after'] in text:
            assert text.count(hunk['after']) == 1, (path, 'ambiguous Step 15C hunk')
            text = text.replace(hunk['after'], hunk['before'], 1)
    return text
