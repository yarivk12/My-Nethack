# My-NetHack modification notice, added 2026-10-15.
# Local project integration/modifications; original authorship is retained.
# Recorded project change dates (Git author dates): 2026-10-06, 2026-10-08.
# See doc/license-compliance.md and doc/my-nethack-modifications.json.
# Existing copyright, attribution, license and warranty notices still apply.
"""Reverse only exact, reviewed Step 19 hunks for historical contracts."""
import json
from pathlib import Path
from step20_source_projection import project as project_step20

MANIFEST = json.loads(Path(__file__).with_name('step19_historical_changes.json').read_text())

def project(path, text):
    text = project_step20(path, text)
    for hunk in MANIFEST.get(path, {}).get('hunks', []):
        if hunk['after'] in text:
            assert text.count(hunk['after']) == 1, (path, 'ambiguous Step 19 hunk')
            text = text.replace(hunk['after'], hunk['before'], 1)
    return text
