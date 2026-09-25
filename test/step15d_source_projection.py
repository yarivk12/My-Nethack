"""Reverse frozen Step 15D hunks; old source contracts retain exact byte gates."""
from step16a_source_projection import project as step16a_project
import json
from pathlib import Path

CHANGES = json.loads(Path(__file__).with_name('step15d_historical_changes.json').read_text())

def project(path, text):
    text = step16a_project(path, text)
    for hunk in CHANGES.get(path, []):
        if hunk['after'] in text:
            assert text.count(hunk['after']) == 1, (path, 'ambiguous Step 15D hunk')
            text = text.replace(hunk['after'], hunk['before'], 1)
    return text
