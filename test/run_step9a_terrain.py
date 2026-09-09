"""Actual ice-wall fire, digging and crystal-pick tests in a wizard arena."""
import re
import sys
from run_step8a_runtime import Game

release,output=sys.argv[1:3]
game=Game(release,output)
try:
    game.lua('nh.debug_flags({hunger=false,mongen=false})')
    (game.path/'ice.lua').write_text('''
des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip", "mazelevel")
des.map([[
---------------
|.............|
|...U...Y.....|
|.............|
|...U...Y.....|
|.............|
---------------
]])
des.region(selection.area(1,1,13,5),"lit")
des.stair("down",1,1)
local x,y=nh.abscoord(0,0);nh.variable("ice_origin",{x=x,y=y})
''')
    game.send('#wizloaddes\n');game.wait('Load which des lua file?')
    game.send('ice.lua\n',2);game.settle()
    text=game.lua('''
for _,name in ipairs({"fire","digging"}) do
 local o=obj.new("wand of "..name.." (0:20)");u.giveobj(o)
 nh.pline("TOOL "..name.." "..o:totable().invlet)
end
for _,name in ipairs({"pick-axe","crystal pick"}) do
 local o=obj.new("uncursed "..name);u.giveobj(o)
 nh.pline("TOOL "..name.." "..o:totable().invlet)
end''')
    letters=dict(re.findall(r'TOOL (fire|digging|pick-axe|crystal pick) (.)',text))
    assert len(letters)==4,text
    def teleport(x,y):
        text=game.lua('local p=nh.variable("ice_origin");nh.pline(string.format("TARGET %%d %%d %%d %%d",u.ux,u.uy,p.x+%d,p.y+%d))'%(x,y))
        a,b,tx,ty=map(int,re.findall(r'TARGET (\d+) (\d+) (\d+) (\d+)',text)[-1])
        if (a,b)!=(tx,ty):
            game.send('\x14');game.wait('Where do you want to be teleported?');game.settle()
            game.send(('l' if tx>a else 'h')*abs(tx-a)+('j' if ty>b else 'k')*abs(ty-b)+'.')
            game.settle()
        assert game.state()[2:]==(tx,ty)
    def check(x,y,typ):
        game.lua('local p=nh.variable("ice_origin");local ox,oy=nh.abscoord(0,0);assert(nh.getmap(p.x+%d-ox,p.y+%d-oy).mapchr=="%s","wrong ice terrain")'%(x,y,typ))
    def zap(name):
        game.send('z'+letters[name]);game.wait('In what direction?');game.send('l');game.settle()
    def dig(name):
        game.send('a'+letters[name]);game.wait('In what direction');game.send('l',2);game.settle()
    teleport(3,2);zap('digging');check(4,2,'U')
    zap('fire');check(4,2,'I')
    # The fire beam can continue through the melted first wall and reach
    # the crystal wall. Restore it before the independent digging check.
    game.lua('des.map({x=1,y=0,map="x"});local p=nh.variable("ice_origin");local ox,oy=nh.abscoord(0,0);des.terrain(p.x+8-ox,p.y+2-oy,"Y")')
    teleport(7,2);zap('digging');check(8,2,'Y')
    zap('fire');check(8,2,'I')
    print('PASS fire melts both ice-wall types; digging beams cannot remove either',flush=True)
    teleport(3,4);dig('pick-axe');check(4,4,'I')
    teleport(7,4);dig('pick-axe');check(8,4,'Y')
    # The palace marks its whole map non-diggable. The crystal pick must
    # still shatter crystal ice there, as in the pinned donor.
    game.lua('des.map({x=1,y=0,map="x"});local p=nh.variable("ice_origin");local ox,oy=nh.abscoord(0,0);des.non_diggable(selection.area(p.x+8-ox,p.y+4-oy,p.x+8-ox,p.y+4-oy))')
    dig('crystal pick');check(8,4,'I')
    print('PASS ordinary pick vs ice/crystal; crystal pick shatters palace-style non-diggable crystal',flush=True)
    before=game.state();game.save();game.close()
    game=Game(release,output,restore=True)
    assert game.state()==before
    check(8,4,'I')
    print('PASS altered ice terrain and crystal-pick inventory persist through save/restore',flush=True)
finally:
    game.close()
