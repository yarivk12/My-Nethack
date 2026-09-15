"""Rerun existing Step 6B/7 Lua checks in their real randomized branches."""
from pathlib import Path
import re
import sys
from run_step8a_runtime import Game
release,output=sys.argv[1:3]
game=Game(release,output)
try:
    game.send('#levelchange\n');game.wait('To what experience level');game.send('30\n');game.settle()
    game.lua('nh.debug_flags({hunger=false})')
    game.send('#wizwhere\n');text=game.wait('Floating branches',more=False)
    depths=[int(re.findall('Stair to '+name+r': (\d+)',text)[0]) for name in ['The Lost Tomb','The Temple of Moloch']]
    game.send('\x1b');game.settle()
    for depth,script in zip(depths,['test_step7.lua','test_step6b.lua']):
        game.send('\x16');game.wait('To what level');game.send(str(depth)+'\n',1);game.settle()
        game.stair('down',True)
        code=Path(__file__).with_name(script).read_text()
        game.lua(code)
        before=game.state();game.save();game=Game(release,output,restore=True)
        assert game.state()==before
        game.lua(code);game.stair('up',True)
        print('PASS existing '+script+' in real branch, save/reload and return',flush=True)
finally:
    game.close()
