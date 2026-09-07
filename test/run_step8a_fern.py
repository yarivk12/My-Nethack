"""Real fern/spore creation and lifecycle; isolated wizard arena."""
from pathlib import Path
import re
import sys
from run_step8a_runtime import Game

release,output=sys.argv[1:3]
game=Game(release,output)
try:
    game.send('#levelchange\n');game.wait('To what experience level');game.send('30\n');game.settle()
    game.lua('nh.debug_flags({hunger=false})')
    (game.path/'ferns.lua').write_text('''
des.level_init({style="solidfill",fg=" "})
des.level_flags("noflipx","noflipy","mazelevel")
des.map([[
---------------
|.............|
|.............|
|...MMM.......|
|.............|
|.............|
---------------
]])
des.region(selection.area(1,1,13,5),"lit")
des.stair("down",1,1)
local x,y=nh.abscoord(7,3);nh.variable("fern_target",{x=x,y=y})
des.monster({id="swamp fern",coord={4,3},peaceful=false,asleep=false})
des.monster({id="swamp fern sprout",coord={5,3},peaceful=false,asleep=false})
des.monster({id="swamp fern spore",coord={6,3},peaceful=false,asleep=false})
''')
    game.send('#wizloaddes\n');game.wait('Load which des lua file?');game.send('ferns.lua\n',2);game.settle()
    game.send('#wizborn\n');text=game.wait('swamp fern',more=False)
    game.wait('--More--',more=False)
    text=game.text()
    assert 'swamp fern sprout' in text and 'swamp fern spore' in text
    game.send('\x1b');game.settle()
    text=game.lua('local p=nh.variable("fern_target");nh.pline(string.format("FERN_TARGET %d %d %d %d",u.ux,u.uy,p.x,p.y))')
    x,y,tx,ty=map(int,re.findall(r'FERN_TARGET (\d+) (\d+) (\d+) (\d+)',text)[-1])
    if (x,y)!=(tx,ty):
        game.send('\x14');game.wait('Where do you want to be teleported?');game.settle()
        game.send(('l' if tx>x else 'h')*abs(tx-x)+('j' if ty>y else 'k')*abs(ty-y)+'.');game.settle()
    assert game.state()[2:]==(tx,ty)
    saw_blast=False
    for _ in range(60):
        game.send('m.',.5)
        saw_blast |= 'duck some of the blast.' in game.text() or 'get blasted!' in game.text()
        if 'Die? [yn]' in game.text():
            game.send('n') # controlled wizard fixture, not a combat simulation
        game.settle()
    assert saw_blast, 'spores never applied their physical contact blast'
    game.send('#wizborn\n');game.wait('--More--',more=False);text=game.text()
    (game.path/'fern-census.txt').write_text(text,encoding='utf8')
    spore=re.search(r'(\d+)\s+(\d+)\s+swamp fern spore',text)
    assert spore and int(spore[2])>1 and int(spore[1])>0,text
    for name in ['swamp fern','swamp fern sprout']:
        match=re.search(r'(\d+)\s+(\d+)\s+'+name+r'\s*\n',text)
        assert match and int(match[1])==0,text
    game.send('\x1b');game.settle()
    print('PASS real fern family creation, repeated spore release/death, surviving plants under spore explosions/clouds',flush=True)
finally:
    game.close()
