# My-NetHack modification notice, added 2026-10-15.
# Local project integration/modifications; original authorship is retained.
# Recorded project change dates (Git author dates): 2026-09-25.
# See doc/license-compliance.md and doc/my-nethack-modifications.json.
# Existing copyright, attribution, license and warranty notices still apply.
"""Frozen Step 16A deltas, preserving the earlier whole-file mutation gates."""
from step16b_source_projection import project as step16b_project
import json
from pathlib import Path
CHANGES = json.loads(Path(__file__).with_name('step16a_historical_changes.json').read_text())
def project(path, text):
    text = step16b_project(path, text)
    for hunk in CHANGES.get(path, []):
        if hunk['after'] in text:
            assert text.count(hunk['after']) == 1, (path, 'ambiguous Step 16A hunk')
            text = text.replace(hunk['after'], hunk['before'], 1)
    return text
