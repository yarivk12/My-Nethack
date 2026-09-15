"""Excavate the real terminal Balin grave and round-trip its Earthstone."""
import re
import sys
from pathlib import Path
from run_step8a_runtime import Game

release,output=sys.argv[1:3]
game=Game(release,output)
try:
    game.send('#levelchange\n');game.wait('To what experience level');game.send('30\n');game.settle()
    game.lua('nh.debug_flags({hunger=false})')
    moria_depth=game.branch_depth('The Ruins of Moria')
    game.send('\x16');game.wait('To what level');game.send(str(moria_depth)+'\n',1);game.settle()
    # The DoD entrance connects to Moria6 through an upward branch stair;
    # return to the main dungeon through Moria6's downward branch stair.
    game.stair('up',True,11)
    game.stair('down',True,0)
    tomb_depth=game.branch_depth('The Lost Tomb')
    # Return to the scheduler-selected DoD entrance and take the named
    # downward branch stair, tolerating the wizard depth prompt's one-level
    # offset across builds.
    game.branch_stair('The Lost Tomb','down',tomb_depth)
    game.lua(Path(__file__).with_name('test_step7.lua').read_text())
    text=game.lua('''
local s=nh.stairways()[1]
local x=s.x+(s.x<40 and 2 or -2)
local y=s.y+(s.y>10 and -14 or 14)
local ox,oy=nh.abscoord(0,0)
assert(nh.getmap(x-ox,y-oy).typ_name=="grave")
local wand=obj.new("wand of digging");u.giveobj(wand)
nh.pline(string.format("GRAVE %d %d %d %d %s",u.ux,u.uy,x,y,wand:totable().invlet))
''')
    match=re.findall(r'GRAVE (\d+) (\d+) (\d+) (\d+) (.)',text)[-1]
    x,y,tx,ty=map(int,match[:4]);letter=match[4]
    game.send('\x14');game.wait('Where do you want to be teleported?');game.settle()
    game.send(('l' if tx>x else 'h')*abs(tx-x)+('j' if ty>y else 'k')*abs(ty-y)+'.');game.settle()
    assert game.state()[2:]==(tx,ty)
    game.send('z'+letter);game.wait('In what direction?');game.send('>');game.settle()
    game.lua('''
local o=obj.next();local corpse,axe,stone=false,false,false
while not o:isnull() do
 local t=o:totable()
 if t.oname=="Balin" then assert(t.corpsenm_name=="dwarf leader");corpse=true end
 if t.otyp_name=="battle-axe" and t.cursed~=0 then axe=true end
 if t.oname=="The Earthstone" then stone=true;u.giveobj(o);break end
 o=o:next()
end
-- Check corpse/axe independently because picking up the stone unlinks it.
o=obj.next()
while not o:isnull() do
 local t=o:totable()
 if t.oname=="Balin" then assert(t.corpsenm_name=="dwarf leader");corpse=true end
 if t.otyp_name=="battle-axe" and t.cursed~=0 then axe=true end
 o=o:next()
end
assert(corpse and axe and stone,"Balin grave reward incomplete: corpse="..tostring(corpse).." axe="..tostring(axe).." stone="..tostring(stone))
''')
    before=game.state();game.save();game=Game(release,output,restore=True)
    assert game.state()==before
    game.lua('''
local o=u.inventory;local found=false
while not o:isnull() do if o:totable().oname=="The Earthstone" then found=true end;o=o:next() end
assert(found,"excavated Earthstone lost in save/reload")
''')
    print('PASS actual terminal Balin grave: named dwarf leader, cursed battle-axe, Earthstone and terminal save/reload',flush=True)
finally:
    game.close()
