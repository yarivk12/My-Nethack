"""Complete packaged Dragon Caves traversal/save checks in actual wizard mode."""
from pathlib import Path
import re
import sys
from run_step8a_runtime import Game

release,output=sys.argv[1:3]
game=Game(release,output)
try:
    game.send('#levelchange\n');game.wait('To what experience level')
    game.send('30\n');game.settle()
    game.lua('nh.debug_flags({hunger=false})')
    assert game.branch_depth('The Dragon Caves')==109
    caves=game.dungeon_number('The Dragon Caves')
    game.send('\x16');game.wait('To what level');game.send('109\n',1);game.settle()
    game.stair('down',True,caves)
    def check():
        game.lua(Path(__file__).with_name('test_step9b.lua').read_text())
    def secure():
        game.lua('nh.debug_flags({hunger=false,mongen=false});nh.debug_flags({mongen=true})')
    check();secure()
    game.save();game.close();game=Game(release,output,restore=True)
    game.lua('nh.debug_flags({hunger=false})');check()
    print('PASS Dragon Caves entrance map save/restore',flush=True)
    for level in range(2,5):
        assert game.stair('down')[:2]==(caves,level)
        check();secure()
    before=game.state();game.save();game.close();game=Game(release,output,restore=True)
    game.lua('nh.debug_flags({hunger=false})');assert game.state()==before;check()
    print('PASS Dragon Caves terminal map save/restore',flush=True)
    for level in range(3,0,-1):
        assert game.stair('up')[:2]==(caves,level)
    assert game.stair('up',True,0)[:2]==(0,109)
    print('PASS four descending Dragon Caves maps, complete round trip to DoD109',flush=True)
finally:
    game.close()
