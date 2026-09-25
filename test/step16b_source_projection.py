"""Frozen Step 16B deltas preserve every earlier mutation guard."""
import json
from pathlib import Path
CHANGES = json.loads(Path(__file__).with_name('step16b_historical_changes.json').read_text())
def project(path, text):
    for hunk in CHANGES.get(path, []):
        if hunk['after'] in text:
            assert text.count(hunk['after']) == 1, (path, 'ambiguous Step 16B hunk')
            text = text.replace(hunk['after'], hunk['before'], 1)
    return text
