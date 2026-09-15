"""Actual wand/terrain and boulder interactions in an isolated wizard arena."""
from pathlib import Path
import re
import sys
from run_step8a_runtime import Game
release,output=sys.argv[1:3]
game=Game(release,output)
try:
    game.lua('nh.debug_flags({hunger=false,mongen=false})')
    (game.path/'terrain.lua').write_text('\n'.join([
        'des.level_init({style="solidfill",fg=" "})',
        'des.level_flags("noflipx","noflipy","mazelevel")',
        'des.map([[', '---------------','|.............|','|...T...M.....|',
        '|.............|','|...t...M.....|','|.............|','---------------',']])',
        'des.region(selection.area(1,1,13,5),"lit")','des.stair("down",1,1)',
        'des.object({id="boulder",coord={7,4}})',
        'local x,y=nh.abscoord(0,0);nh.variable("arena_origin",{x=x,y=y})']))
    game.send('#wizloaddes\n');game.wait('Load which des lua file?');game.send('terrain.lua\n',2);game.settle()
    text=game.lua('''
for _,name in ipairs({"cold","fire","death"}) do
 local o=obj.new("wand of "..name.." (0:20)");u.giveobj(o)
 nh.pline("WAND "..name.." "..o:totable().invlet)
end
''')
    letters=dict(re.findall(r'WAND (cold|fire|death) (.)',text))
    assert len(letters)==3,text
    def teleport(x,y):
        text=game.lua('local p=nh.variable("arena_origin");nh.pline(string.format("TARGET %%d %%d %%d %%d",u.ux,u.uy,p.x+%d,p.y+%d))'%(x,y))
        a,b,tx,ty=map(int,re.findall(r'TARGET (\d+) (\d+) (\d+) (\d+)',text)[-1])
        if (a,b)!=(tx,ty):
            game.send('\x14');game.wait('Where do you want to be teleported?');game.settle()
            game.send(('l' if tx>a else 'h')*abs(tx-a)+('j' if ty>b else 'k')*abs(ty-b)+'.');game.settle()
        assert game.state()[2:]==(tx,ty)
    def check(x,y,typ):
        game.lua('local p=nh.variable("arena_origin");local ox,oy=nh.abscoord(0,0);assert(nh.getmap(p.x+%d-ox,p.y+%d-oy).typ_name=="%s","terrain mismatch")'%(x,y,typ))
    def zap(name):
        game.send('z'+letters[name]);game.wait('In what direction?');game.send('l');game.settle()
    teleport(7,2);zap('cold');check(8,2,'ice')
    zap('fire');check(8,2,'muddy swamp')
    zap('fire');check(8,2,'room')
    teleport(3,2);zap('death');check(4,2,'dead tree')
    zap('fire');check(4,2,'room')
    teleport(3,4);zap('fire');check(4,4,'room')
    # Restore this bog after the preceding fire beam crossed it.
    game.lua('local p=nh.variable("arena_origin");local ox,oy=nh.abscoord(0,0);des.terrain(p.x+8-ox,p.y+4-oy,"M")')
    teleport(6,4);game.send('l');game.settle();check(8,4,'room')
    print('PASS actual bog freeze/thaw/evaporation, living-tree death, dead-tree burning and boulder filling',flush=True)
finally:
    game.close()
