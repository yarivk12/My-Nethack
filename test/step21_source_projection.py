"""Reverse the exact Step 21 save-epoch change for older epoch contracts."""

def project(path, text):
    if path == 'include/patchlevel.h':
        text = text.replace('#define EDITLEVEL 17\n', '#define EDITLEVEL 16\n', 1)
    return text
