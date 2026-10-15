# My-NetHack modification notice, added 2026-10-15.
# Local project integration/modifications; original authorship is retained.
# Recorded project change dates (Git author dates): 2026-10-15.
# See doc/license-compliance.md and doc/my-nethack-modifications.json.
# Existing copyright, attribution, license and warranty notices still apply.
"""Run the unchanged Step 21 production assertions with robust UI selection.

Permanent identification may reassign inventory letters. Refresh the UI
mapping after that native command; retain every historical assertion.
"""
from pathlib import Path
import ctypes
import os

if os.name == 'nt':
    ctypes.windll.kernel32.SetErrorMode(0x0001 | 0x0002 | 0x8000)
original = Path(__file__).with_name('run_step21_save.py')
source = original.read_text(encoding='utf8')
before = '    items = inventory(g)\n'
after = '    # Full identification uses the production wizard command, not a debug setter.\n'
assert source.count(before) == 1 and source.count(after) == 1
source = source.replace(before, '', 1)
source = source.replace(after, before + "    print('Native identified artifact slots: '+repr(items), flush=True)\n" + after, 1)
helpers = '''
def select_artifact_menu(g, letter):
    name = next(name for name, slot in items.items() if slot[0] == letter)
    if name.startswith('The '): name = name[4:]
    g.send(':'); g.wait('Search for:'); g.send(name.upper()+'\\n')

def invoke_selected_artifact(g, letter):
    g.send('#INVOKE\\n'); g.wait('invoke?'); g.send('*')
    g.settle(); select_artifact_menu(g, letter)

'''
# Native case-insensitive menu search avoids WinPTY's lowercase key issue.
# Selection still goes through the production inventory menu and command.
anchor = 'g = Game(release, output)\n'
assert source.count(anchor) == 1
source = source.replace(anchor, helpers + anchor, 1)
for old, new in (
    ("g.send('#INSPECT\\n'); g.wait('inspect?'); g.send(letter)",
     "g.send('#INSPECT\\n'); g.wait('inspect?'); select_artifact_menu(g, letter)"),
    ("g.send('#INVOKE\\n'); g.wait('invoke?'); g.send(book)",
     "invoke_selected_artifact(g, book)"),
    ("g.send('#INVOKE\\n'); g.wait('invoke?'); g.send(saddle); g.settle()",
     "invoke_selected_artifact(g, saddle); g.settle()"),
    ("g.wait('Use the Book'); g.send('t'); g.settle()",
     "g.wait('Use the Book'); g.send(':'); g.wait('Search for:'); g.send('TURN PAGE\\n'); g.settle()"),
):
    assert source.count(old) == 1, old
    source = source.replace(old, new, 1)
exec(compile(source, str(original), 'exec'), {'__name__': '__main__', '__file__': str(original)})
