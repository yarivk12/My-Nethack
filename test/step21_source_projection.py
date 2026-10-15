# My-NetHack modification notice, added 2026-10-15.
# Local project integration/modifications; original authorship is retained.
# Recorded project change dates (Git author dates): 2026-10-10.
# See doc/license-compliance.md and doc/my-nethack-modifications.json.
# Existing copyright, attribution, license and warranty notices still apply.
"""Reverse the exact Step 21 save-epoch change for older epoch contracts."""

def project(path, text):
    if path == 'include/patchlevel.h':
        text = text.replace('#define EDITLEVEL 17\n', '#define EDITLEVEL 16\n', 1)
    return text
