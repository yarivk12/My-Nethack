"""Reverse frozen Step 15D hunks; old source contracts retain exact byte gates."""
import json
from pathlib import Path

CHANGES = json.loads(Path(__file__).with_name('step15d_historical_changes.json').read_text())

def project(path, text):
    for hunk in CHANGES.get(path, []):
        if hunk['after'] in text:
            assert text.count(hunk['after']) == 1, (path, 'ambiguous Step 15D hunk')
            text = text.replace(hunk['after'], hunk['before'], 1)
    return text
