"""Reverse only exact, reviewed Step 19 hunks for historical contracts."""
import json
from pathlib import Path

MANIFEST = json.loads(Path(__file__).with_name('step19_historical_changes.json').read_text())

def project(path, text):
    for hunk in MANIFEST.get(path, {}).get('hunks', []):
        if hunk['after'] in text:
            assert text.count(hunk['after']) == 1, (path, 'ambiguous Step 19 hunk')
            text = text.replace(hunk['after'], hunk['before'], 1)
    return text
