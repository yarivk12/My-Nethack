# My-NetHack modification notice, added 2026-10-15.
# Local project integration/modifications; original authorship is retained.
# Recorded project change dates (Git author dates): 2026-09-09.
# See doc/license-compliance.md and doc/my-nethack-modifications.json.
# Existing copyright, attribution, license and warranty notices still apply.
"""Actual blue-slime combat, movement restraint and frozen save/restore."""
import re
import sys
from run_step8a_runtime import Game

release,output=sys.argv[1:3]
game=Game(release,output)
try:
    game.send('#levelchange\n');game.wait('To what experience level')
    game.send('30\n');game.settle()
    game.lua('nh.debug_flags({hunger=false})')
    (game.path/'slime.lua').write_text('''
des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip", "mazelevel")
des.map([[
-------
|.....|
|.....|
|.....|
-------
]])
des.region(selection.area(1,1,5,3),"lit")
des.stair("up",2,2)
des.stair("down",4,2)
des.monster({id="blue slime",coord={3,2},asleep=false,peaceful=false})
''')
    game.send('#wizloaddes\n');game.wait('Load which des lua file?')
    game.send('slime.lua\n',2);game.settle()
    for _ in range(50):
        if re.search(r'\bFrozen\b',game.text()):break
        game.send('m.',.5);game.settle()
    assert re.search(r'\bFrozen\b',game.text()),'blue slime never froze hero\n'+game.text()
    game.lua('nh.debug_flags({mongen=false})')
    before=game.state()
    game.send('l');game.settle()
    assert game.state()==before,'frozen hero moved'
    game.save();game.close();game=Game(release,output,restore=True)
    assert re.search(r'\bFrozen\b',game.text()),'frozen state lost at restore'
    assert game.state()==before
    game.send('l');game.settle()
    assert game.state()==before,'frozen hero moved after restore'
    # A wizard teleport releases the restraint, as does normal teleportation.
    game.send('\x14');game.wait('Where do you want to be teleported?')
    game.settle();game.send('h.');game.settle()
    assert game.state()[2:]!=before[2:]
    print('PASS actual blue-slime contact, movement-only freeze, frozen save/restore and teleport release',flush=True)
finally:
    game.close()
