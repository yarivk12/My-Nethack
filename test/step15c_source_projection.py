"""Reverse only frozen Step 15C production hunks for earlier identity gates.

The Step 15B/audit manifests and original baselines remain immutable.
test_step15c_source.py rejects mutations inside and outside these hunks.
"""
import json
from pathlib import Path

CHANGES = json.loads(Path(__file__).with_name(
    'step15c_historical_changes.json').read_text(encoding='utf8'))


def project(path, text):
    for hunk in CHANGES.get(path, []):
        if hunk['after'] in text:
            assert text.count(hunk['after']) == 1, (path, 'ambiguous Step 15C hunk')
            text = text.replace(hunk['after'], hunk['before'], 1)
    return text
