"""Operate the real stethoscope lock occupation and save a loaded iron safe."""
import re
import sys
from run_step8a_runtime import Game

release,output=sys.argv[1:3]
game=Game(release,output)
check='''
local ox,oy=nh.abscoord(0,0)
local o=obj.at(u.ux-ox,u.uy-oy)
while not o:isnull() and o:totable().otyp_name~="iron safe" do o=o:next(true) end
assert(not o:isnull(),"missing iron safe")
local t=o:totable()
'''
try:
    text=game.lua('''
nh.debug_flags({mongen=false,hunger=false})
nh.parse_config("OPTIONS=autounlock:none")
local ox,oy=nh.abscoord(0,0)
local safe=obj.new("uncursed untrapped locked empty iron safe")
assert(safe:totable().olocked~=0 and safe:totable().otrapped==0)
safe:addcontent(obj.new("small piece of unrefined mithril"))
safe:placeobj(u.ux-ox,u.uy-oy)
local scope=obj.new("stethoscope");u.giveobj(scope)
nh.pline("SCOPE "..scope:totable().invlet)
''')
    letter=re.findall(r'SCOPE (.)',text)[-1]
    unlocked=False
    for _ in range(8):
        game.send('a'+letter);game.wait('In what direction?');game.send('.')
        game.wait('crack it?');game.send('y',3);game.settle()
        text=game.lua(check+'nh.pline("LOCKSTATE "..t.olocked)')
        if 'LOCKSTATE 0' in text:
            unlocked=True;break
    assert unlocked,'stethoscope never opened safe'
    game.lua(check+'''
assert(t.olocked==0 and t.has_contents~=0)
assert(o:contents():totable().otyp_name=="small piece of unrefined mithril")
''')
    game.save();game=Game(release,output,restore=True)
    game.lua(check+'''
assert(t.olocked==0 and t.has_contents~=0)
assert(o:contents():totable().otyp_name=="small piece of unrefined mithril")
''')
    print('PASS actual stethoscope safe unlocking; mithril contents and unlocked state survive save/reload',flush=True)
finally:
    game.close()
