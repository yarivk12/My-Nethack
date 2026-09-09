"""Actual glowing-dragon polymorph breath and terrain persistence."""
import re
import sys
from run_step8a_runtime import Game,tty_transcript

release,output=sys.argv[1:3]
game=Game(release,output)
try:
    game.send('#levelchange\n');game.wait('To what experience level');game.send('30\n');game.settle()
    game.lua('nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()')
    (game.path/'lava.lua').write_text('''
des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip","mazelevel")
des.map([[
-------------------
|.................|
|...|.............|
|.................|
|...Y.............|
|.................|
|...U.............|
|.................|
|...|.............|
|.................|
-------------------
]])
des.region(selection.area(1,1,17,9),"lit")
des.non_diggable(selection.area(4,8,4,8))
des.stair("up",3,2);des.stair("down",1,1)
local x,y=nh.abscoord(0,0);nh.variable("lava_origin",{x=x,y=y})
''')
    game.send('#wizloaddes\n');game.wait('Load which des lua file?');game.send('lava.lua\n',2);game.settle()
    game.send('#polyself\n');game.wait('Become what kind of monster?')
    game.send('glowing dragon\n',2);game.settle()
    game.lua('assert(nh.int_to_pmname(u.umonnum)=="glowing dragon")')
    def teleport(x,y):
        text=game.lua('local p=nh.variable("lava_origin");nh.pline(string.format("TARGET %%d %%d %%d %%d",u.ux,u.uy,p.x+%d,p.y+%d))'%(x,y))
        a,b,tx,ty=map(int,re.findall(r'TARGET (\d+) (\d+) (\d+) (\d+)',text)[-1])
        if (a,b)!=(tx,ty):
            game.send('\x14');game.wait('Where do you want to be teleported?');game.settle()
            game.send(('l' if tx>a else 'h')*abs(tx-a)+('j' if ty>b else 'k')*abs(ty-b)+'.');game.settle()
        assert game.state()[2:]==(tx,ty)
    def breath():
        game.send('#monster\n');game.wait('In what direction?');game.send('l');game.settle()
    def check(y,typ):
        game.lua('local p=nh.variable("lava_origin");local ox,oy=nh.abscoord(0,0);assert(nh.getmap(p.x+4-ox,p.y+%d-oy).mapchr=="%s","wrong lava breath terrain")'%(y,typ))
    if '--reflection-only' not in sys.argv[3:]:
        for row,typ in [(2,'L'),(4,'I'),(6,'I'),(8,'|')]:
            teleport(3,row);breath();check(row,typ)
        game.save();game.close();game=Game(release,output,restore=True)
        for row,typ in [(2,'L'),(4,'I'),(6,'I'),(8,'|')]:check(row,typ)
        print('PASS actual glowing dragon lava breath: walls to lava, both ice walls melt, nondiggable wall preserved, save/restore',flush=True)
    # A native silver dragon reflects the lava jet. The donor jet stops;
    # it must reach neither a wall behind the target nor behind the hero.
    game.lua('''nh.debug_flags({mongen=true})
des.map({x=1,y=0,map="x"});local p=nh.variable("lava_origin");local ox,oy=nh.abscoord(0,0)
for _,x in ipairs({2,8}) do des.terrain(p.x+x-ox,p.y+2-oy,"|") end
des.terrain(p.x+4-ox,p.y+2-oy,".")''')
    teleport(3,2)
    game.lua('''des.map({x=1,y=0,map="x"});local ox,oy=nh.abscoord(0,0)
for dx=1,4 do des.terrain(u.ux+dx-ox,u.uy-oy,".") end
des.monster({id="silver dragon",coord={u.ux+2-ox,u.uy-oy},paralyzed=127,peaceful=false})''')
    # Level-design terrain writes do not refresh the running vision grid.
    # Restore the prepared fixture so visibility matches its new corridor.
    game.save();game.close();game=Game(release,output,restore=True)
    for attempt in range(16):
        game.lua('''des.map({x=1,y=0,map="x"})
local p=nh.variable("lava_origin");local ox,oy=nh.abscoord(0,0)
for _,x in ipairs({2,8}) do des.terrain(p.x+x-ox,p.y+2-oy,"|") end
assert(nh.getmap(u.ux-ox,u.uy-oy).mapchr==".","reflection fixture changed hero floor")''')
        start=len(game.raw);breath()
        if ('reflect' in tty_transcript(''.join(game.raw[start:]))
                or 'reflect' in game.text()):break
        # A normally missed ray can pass the reflector. Reset the target
        # walls and retry until actual reflection, rather than asserting
        # that every attack roll must hit.
    else:raise AssertionError('no reflected lava hit in 16 attempts\n'+game.text())
    game.lua('''local p=nh.variable("lava_origin");local ox,oy=nh.abscoord(0,0)
for _,x in ipairs({2,8}) do assert(nh.getmap(p.x+x-ox,p.y+2-oy).mapchr=="|","lava continued after reflection") end''')
    print('PASS actual silver-dragon reflection stops lava without bouncing or continuing',flush=True)
finally:
    game.close()
