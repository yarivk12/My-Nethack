"""Actual packaged terminal Watcher creation, unique death and lamp reward."""
import re
import sys
from run_step8a_runtime import Game

release,output=sys.argv[1:3]
game=Game(release,output)
try:
    from pathlib import Path
    source=(Path(__file__).resolve().parents[1]/'dat/moria6-1.lua').read_text()
    source += '\ndes.level_flags("noflipx","noflipy")\nlocal x,y=nh.abscoord(66,10);nh.variable("watcher_coord",{x=x,y=y})\n'
    (game.path/'watcher.lua').write_text(source)
    game.send('#wizloaddes\n');game.wait('Load which des lua file?')
    game.send('watcher.lua\n',2);game.settle()
    text=game.lua('local p=nh.variable("watcher_coord");nh.pline(string.format("WATCHER %d %d %d %d",u.ux,u.uy,p.x,p.y))')
    x,y,tx,ty=map(int,re.findall(r'WATCHER (\d+) (\d+) (\d+) (\d+)',text)[-1])
    game.send('#wizkill\n');game.wait('Pick first monster to slay');game.settle()
    game.send(('l' if tx>x else 'h')*abs(tx-x)+('j' if ty>y else 'k')*abs(ty-y)+'.')
    game.wait('Next monster');game.send('\x1b');game.settle()
    def census():
        game.send('#wizborn\n');text=game.wait('Watcher in the Water',more=False)
        assert re.search(r'1\s+1\s+E\s+Watcher in the Water',text),text
        game.send('\x1b');game.settle()
    census()
    check='''
local o=obj.next();local found=false
while not o:isnull() do
 local t=o:totable()
 if t.otyp_name=="magic lamp" then assert(t.spe==1 and t.cursed==0 and t.blessed==0);found=true end
 o=o:next()
end
assert(found,"Watcher did not drop its uncursed magic lamp")
'''
    game.lua(check);game.save();game=Game(release,output,restore=True)
    census();game.lua(check)
    print('PASS Watcher unique birth/death, uncursed charged vanilla magic lamp drop, save/reload',flush=True)
finally:
    game.close()
