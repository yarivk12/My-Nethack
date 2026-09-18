"""Exact Step 13 exclusions for historical whole-file scope contracts.

The frozen manifest contains only reviewed enhancement/build/test-hook hunks
against f27b7f20. It is not generated at test time: any changed hunk fails,
and every byte outside these hunks remains subject to the older baseline.
Step 13's own source and native runtime gates test the excluded behavior.
"""
import json
from pathlib import Path

CHANGES = json.loads(Path(__file__).with_name(
    'step13_historical_changes.json').read_text(encoding='utf8'))


def project(path, text):
    for hunk in CHANGES.get(path, []):
        after, before = hunk['after'], hunk['before']
        if after in text:
            assert text.count(after) == 1, (path, 'ambiguous Step 13 hunk')
            text = text.replace(after, before, 1)
        else:
            # Some historical gates compose projections on the same file.
            assert before in text, (path, 'changed/missing Step 13 hunk')
    return text
