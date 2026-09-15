"""Paid native uncurse, weapon work and portal guidance across fresh merchants.

Only arena geometry is fixed; service offers retain their production rolls.
"""
import re,sys
from run_step8a_runtime import Game
release,output=sys.argv[1:3]
game=Game(release,output)
completed=set()
def services():
    game.send('p',1);game.wait('Do you wish to try our other services?')
    game.send('y',1);game.wait('Services available:',more=False)
    return game.wait('Guide to portal',more=False)
def state():
    text=game.lua('''local o=u.inventory;while not o:isnull() do
local t=o:totable();if t.otyp_name=="dagger" then
 nh.pline("GEAR "..t.invlet.." "..t.spe.." "..t.cursed.." "..t.oeroded.." "..t.oerodeproof)
end;o=o:next() end''')
    values=re.findall(r'GEAR (.) (-?\d+) (\d+) (\d+) (\d+)',text)[-1]
    return values[0],*map(int,values[1:])
def pay():
    game.wait('Interested?');game.send('y',1);game.settle()
def portals(revealed):
    game.lua('''local n,seen=0,0;local ox,oy=nh.abscoord(0,0)
for x=1,79 do for y=0,20 do
 if nh.getmap(x-ox,y-oy).has_trap then local t=nh.gettrap(x-ox,y-oy)
  if t.ttyp_name=="magic portal" then n=n+1;if t.tseen then seen=seen+1 end end
 end
end end
assert(n>0,"missing native portal")
assert(%s,"wrong portal discovery state")''' % ('seen==n' if revealed else 'seen<n'))
try:
    game.lua('nh.debug_flags({hunger=false,mongen=false})')
    game.send('\x16');game.wait('To what level');game.send('?\n',1)
    key=None
    for _ in range(12):
        match=re.search(r'^\s*(.)\s+[-+]\s+ossa1\b',game.text(),re.M)
        if match:key=match[1];break
        game.send('>',.6)
    assert key,'Elshava missing from wizard menu'
    game.send(key,1);game.settle()
    assert game.state()[:2]==(game.dungeon_number('Mithardir'),1)
    (game.path/'works.lua').write_text('''des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip")
des.map({x=1,y=0,map=[=[
---------
|.......|
|.......|
+.......|
|.......|
|.......|
---------
]=]})
des.door("closed",0,3)
des.region({region={1,1,7,5},lit=1,type="sand-walker shop",filled=1})
des.levregion({region={6,4,6,4},type="portal",name="mith1"})
''',encoding='utf8')
    for attempt in range(32):
        game.lua('nh.debug_flags({mongen=false});nh.debug_flags({mongen=true});u.clear_inventory()')
        game.send('#wizloaddes\n');game.wait('Load which des lua file?')
        game.send('works.lua\n',2);game.settle()
        text=game.lua('''if u.uhunger<500 then local o=obj.new("uncursed food ration")
u.giveobj(o);nh.pline("FOOD "..o:totable().invlet) end''')
        food=re.findall(r'FOOD (.)',text)
        if food:game.send('e'+food[-1],2);game.settle()
        x,y=game.state()[2:]
        game.send('\x14');game.wait('Where do you want to be teleported?');game.settle()
        game.send(('l' if 3>x else 'h')*abs(3-x)+('j' if 3>y else 'k')*abs(3-y)+'.')
        game.settle()
        # A generated portal or the shopkeeper can occupy the requested
        # square. Native wizard teleport then selects an adjacent legal
        # square; require an interior spot still in payment range.
        px,py=game.state()[2:]
        assert 2<=px<=4 and 1<=py<=5,(px,py,game.text())
        game.lua('u.giveobj(obj.new("10000 gold pieces"));u.giveobj(obj.new("cursed very rusty +0 dagger"))')
        letter=state()[0]
        game.send('w'+letter);game.settle() # native welding reveals the curse
        offerings=services();game.send('\x1b');game.settle()
        if 'guide' not in completed:
            # Stock or a branch connector can make native region placement
            # relocate the portal. Inspect its actual generated position.
            portals(False)
            services();game.send('o');pay()
            # trap_detect has an interactive map; leave that display normally.
            game.send('\x1b');game.settle()
            portals(True)
            completed.add('guide');print('PASS paid portal guide reveals actual branch portal',flush=True)
        if 'Uncurse' in offerings and 'uncurse' not in completed:
            services();game.send('u');game.wait('What do you want to uncurse?')
            game.send(letter);pay();assert state()[2]==0
            completed.add('uncurse');print('PASS paid uncurse removes known welded curse',flush=True)
        if 'Weapon-works' in offerings:
            for key,name in [('w','proof'),('e','enchant'),('a','acid')]:
                if name in completed:continue
                services();game.send('w');game.wait('What do you want to improve?')
                game.send(letter);game.wait('Weapon-works:',more=False)
                menu=game.text()
                label={'w':'Ward against damage','e':'Enchant','a':'Acid coating'}[key]
                if label not in menu:
                    game.send('\x1b');game.settle();continue
                game.send(key);pay()
                current=state()
                if name=='proof':assert current[3:]==(0,1)
                elif name=='enchant':assert current[1]==1
                else:
                    game.send('i',1);assert 'acid-coated' in game.text()
                    game.send('\x1b');game.settle()
                completed.add(name);print('PASS paid weapon service:',name,flush=True)
        if completed=={'guide','uncurse','proof','enchant','acid'}:
            before=state();game.save();game.close();game=Game(release,output,restore=True)
            assert state()==before
            print('PASS paid item state save/restore; fresh merchants sampled:',attempt+1,flush=True)
            break
    assert completed=={'guide','uncurse','proof','enchant','acid'},completed
finally:
    game.close()
