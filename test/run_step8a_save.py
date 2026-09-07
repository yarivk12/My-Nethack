"""Real packaged save/reload on the Moria bridge, with new inventory items."""
import sys
from run_step8a_runtime import Game

release, output = sys.argv[1:3]
game=Game(release,output)
snapshot='''
local ox,oy=nh.abscoord(0,0)
local rows={}
for y=0,20 do for x=1,79 do
 rows[#rows+1]=tostring(nh.getmap(x-ox,y-oy).typ)
end end
local state=table.concat(rows,",")
'''
try:
    game.send('#levelchange\n');game.wait('To what experience level');game.send('30\n');game.settle()
    game.lua('nh.debug_flags({hunger=false})')
    moria_depth=game.branch_depth('The Ruins of Moria')
    game.send('\x16');game.wait('To what level');game.send(str(moria_depth)+'\n',1);game.settle()
    game.stair('up',True);game.stair('up')
    game.lua('''
u.giveobj(obj.new("small piece of unrefined mithril"))
u.giveobj(obj.new("magic candle"))
'''+snapshot+'''
nh.variable("step8_map",state)
nh.variable("step8_level",u.dlevel)
''')
    before=game.state()
    game.save()
    game=Game(release,output,restore=True)
    assert game.state()==before
    game.lua(snapshot+'''
assert(nh.variable("step8_map")==state,"terrain did not round trip")
assert(nh.variable("step8_level")==u.dlevel and u.dlevel==5)
local o=u.inventory;local mithril,candle=false,false
while not o:isnull() do
 local t=o:totable()
 if t.otyp_name=="small piece of unrefined mithril" then mithril=true end
 if t.otyp_name=="magic candle" then candle=true end
 o=o:next()
end
assert(mithril and candle,"new item IDs did not round trip")
''')
    game.stair('down');assert game.stair('down',True)[:2]==(0,moria_depth)
    print('PASS real bridge save/restore: exact terrain, position, mithril and Magic Candle; stairs return to DoD%d' % moria_depth,flush=True)
finally:
    game.close()
