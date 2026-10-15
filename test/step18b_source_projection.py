# My-NetHack modification notice, added 2026-10-15.
# Local project integration/modifications; original authorship is retained.
# Recorded project change dates (Git author dates): 2026-10-04.
# See doc/license-compliance.md and doc/my-nethack-modifications.json.
# Existing copyright, attribution, license and warranty notices still apply.
"""Reverse only reviewed Step 18B deltas for historical source gates."""
import json
from pathlib import Path

MANIFEST = json.loads(Path(__file__).with_name('step18b_historical_changes.json').read_text())

def project(path, text):
    for hunk in MANIFEST.get(path, {}).get('hunks', []):
        if hunk['after'] in text:
            assert text.count(hunk['after']) == 1, (path, 'ambiguous Step 18B hunk')
            text = text.replace(hunk['after'], hunk['before'], 1)
    return text
