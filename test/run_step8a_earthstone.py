"""Actual Earthstone invocation, portal persistence and unpaired teleport."""
import re
import sys
from run_step8a_runtime import Game

release,output=sys.argv[1:3]
game=Game(release,output)
check='''
local ox,oy=nh.abscoord(0,0);local n=0;local px,py
for x=1,79 do for y=0,20 do
 if nh.getmap(x-ox,y-oy).has_trap then
  local t=nh.gettrap(x-ox,y-oy)
  if t.ttyp_name=="magic portal" then n=n+1;px=x;py=y end
 end
end end
assert(n==1,"invocation did not leave exactly one portal")
local o=u.inventory
while not o:isnull() do assert(o:totable().oname~="The Earthstone");o=o:next() end
nh.pline(string.format("PORTAL %d %d %d %d",u.ux,u.uy,px,py))
'''
try:
    text=game.lua('''
nh.debug_flags({mongen=false,hunger=false})
local o=obj.new("The Earthstone");u.giveobj(o)
assert(o:totable().oname=="The Earthstone")
nh.pline("STONE "..o:totable().invlet)
''')
    letter=re.findall(r'STONE (.)',text)[-1]
    game.send('#invoke\n');game.wait('What do you want to invoke?');game.send(letter);game.settle()
    game.lua(check);game.save();game=Game(release,output,restore=True)
    text=game.lua(check)
    x,y,px,py=map(int,re.findall(r'PORTAL (\d+) (\d+) (\d+) (\d+)',text)[-1])
    assert max(abs(px-x),abs(py-y))==1
    keys={(-1,-1):'y',(0,-1):'k',(1,-1):'u',(-1,0):'h',(1,0):'l',
          (-1,1):'b',(0,1):'j',(1,1):'n'}
    game.send(keys[px-x,py-y]);game.wait('Really step into that magic portal?')
    game.send('y');game.wait('malfunctioning');game.settle()
    assert game.state()[:2]==(0,1)
    print('PASS consumed Earthstone invocation, portal save/reload, local unpaired teleport without dungeon migration',flush=True)
finally:
    game.close()
