"""Reverse exact reviewed Step 20 deltas for historical source gates.

The frozen manifest is anchored to the committed Step 20 change and its parent.
Matching requires the entire accepted hunk; mutations and unrelated edits remain
visible to the historical assertions and exact baseline hashes.
"""
import json
from pathlib import Path

MANIFEST = json.loads(Path(__file__).with_name('step20_historical_changes.json').read_text())

def reverse(record, path, text):
    for hunk in record.get(path, {}).get('hunks', []):
        if hunk['after'] in text:
            assert text.count(hunk['after']) == 1, (path, 'ambiguous Step 20 hunk')
            text = text.replace(hunk['after'], hunk['before'], 1)
    return text

def project_validation(path, text):
    return reverse(MANIFEST.get('validation_fixes', {}).get('files', {}), path, text)

def project(path, text):
    return reverse(MANIFEST['files'], path, project_validation(path, text))
