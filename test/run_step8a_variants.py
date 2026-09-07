"""Generate all ten packaged resources through the real special-level loader.

Fresh wizard game per map; regular monster generation stays enabled.
This exercises Lua APIs, flags, monsters, objects and map finalization.
Branch connectivity is checked separately by run_step8a_runtime.py.
"""
from pathlib import Path
import sys
from run_step8a_runtime import Game

release, output = sys.argv[1:3]
names=['moria1-1','moria2-1','moria3-1','moria4-1','moria4-2','moria4-3',
       'moria4-4','moria5-1','moria6-1','moria6-2']
for name in names:
    game=Game(release,Path(output)/name)
    try:
        game.send('#wizloaddes\n')
        try:
            game.wait('Load which des lua file?')
        except AssertionError:
            # A fresh process can still be redrawing its welcome screen when
            # the first extended command is sent.  Retry once after a clean
            # redraw so every pinned resource gets the same loader coverage.
            game.send('\x1b')
            game.settle()
            game.send('#wizloaddes\n')
            game.wait('Load which des lua file?')
        game.send(name+'.lua\n',2)
        game.settle()
        game.lua('''
local ox,oy=nh.abscoord(0,0);local walk=0
for x=1,79 do for y=0,20 do
 local m=nh.getmap(x-ox,y-oy)
 if m.typ_name=="room" then walk=walk+1 end
end end
assert(walk>0,"empty generated map")
''')
        (game.path/'generated.txt').write_text(game.text(),encoding='utf8')
    finally:
        game.close()
    print('PASS actual packaged resource generation',name,flush=True)
