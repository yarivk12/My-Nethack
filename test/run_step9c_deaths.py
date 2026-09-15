"""Actual mummy and Aspect birth/death rewards persisted across restore.

Uses native wizard slaying to isolate the real death/drop path from combat.
"""
import re,sys
from run_step8a_runtime import Game as BaseGame
class Game(BaseGame):
    def text(self):
        # wait() dismisses --More-- directly through the PTY. Observe its
        # reconstructed screens too, including the short quest pager panel.
        text=super().text()
        if not hasattr(self,'observations'):self.observations=[]
        if not self.observations or self.observations[-1]!=text:
            self.observations.append(text)
        return text
release,output=sys.argv[1:3]
game=Game(release,output)

def kill():
    x,y=game.state()[2:]
    start=len(game.observations)
    game.send('#wizkill\n');game.wait('Pick first monster to slay');game.settle()
    game.send(('l' if 9>x else 'h')*abs(9-x)+('j' if 3>y else 'k')*abs(3-y)+'.')
    game.wait('Next monster');game.send('\x1b');game.settle()
    if 'putrefies with impossible speed' in '\n'.join(game.observations[start:]):
        # The donor's 1/20 Alabaster life-saving transformation leaves a
        # living blob/pudding. Kill that survivor to inspect its inventory,
        # while the tile count below still requires exactly one shard.
        print('Observed Alabaster putrefaction; slaying the surviving form',flush=True)
        kill()

def restore():
    global game
    game.save();game.close();game=Game(release,output,restore=True)

def energy_drain():
    def state():
        text=game.lua('nh.pline("ENERGY "..u.uen.." "..u.moves)')
        return tuple(map(int,re.findall(r'ENERGY (\d+) (\d+)',text)[-1]))
    before,turn=state()
    assert before>20,'Fixture needs energy to observe the passive drain'
    for _ in range(24):
        game.send('.',.3);game.settle()
        after,now=state()
        if now>=turn+6:break
    assert now>=turn+6 and after<before,(before,after,turn,now)
    # Native Wizard energy regeneration may also fire during these turns;
    # test net loss, while focused code checks the exact three-point drain.
    print('PASS actual Aspect passive energy drain:',before,'->',after,
          'in',now-turn,'world turns',flush=True)

def drops(tiles,slabs):
    return game.lua('''local tile,slab,mask,key=0,0,0,0
local o=obj.next();while not o:isnull() do
 local t=o:totable();local name=t.otyp_name
 if string.find(name,"syllable of",1,true) then tile=tile+t.quan end
 if name=="First Word" or name=="Dividing Word" or name=="Nurturing Word" then slab=slab+t.quan end
 if name=="mask" and t.cursed~=0 then mask=mask+t.quan end
 if t.oname=="The Third Key of Chaos" then key=key+t.quan end
 o=o:next()
end
assert(tile==%d,"wrong mummy tile count: "..tile)
assert(slab==%d,"wrong Aspect slab count: "..slab)
if tile>0 then assert(mask>=1,"missing mummy mask") end
nh.pline("REWARD_KEY "..key)
''' % (tiles,slabs))

try:
    game.lua('nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()')
    game.send('#levelchange\n');game.wait('To what experience level');game.send('30\n');game.settle()
    (game.path/'deaths.lua').write_text('''
des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip")
des.map({x=1,y=0,map=[=[
-----------------
|...............|
|...............|
|...............|
|...............|
|...............|
-----------------
]=]})
des.region(selection.area(1,1,15,5),"lit")
''',encoding='utf8')
    game.send('#wizloaddes\n');game.wait('Load which des lua file?');game.send('deaths.lua\n',2);game.settle()
    x,y=game.state()[2:]
    game.send('\x14');game.wait('Where do you want to be teleported?');game.settle()
    game.send(('l' if 4>x else 'h')*abs(4-x)+('j' if 3>y else 'k')*abs(3-y)+'.')
    game.settle();assert game.state()[2:]==(4,3)
    game.lua('nh.debug_flags({mongen=true});des.monster({id="Alabaster mummy",coord={8,3},peaceful=false,paralyzed=126})')
    drops(0,0);restore();kill();drops(1,0);restore();drops(1,0)
    print('PASS actual mummy alive/dead restore, one syllable drop and cursed mask',flush=True)
    start=len(game.observations)
    game.lua('des.monster({id="aspect of The Silence",coord={8,3},peaceful=false,paralyzed=126})')
    text='\n'.join(game.observations[start:])
    assert 'A terrible silence has fallen!' in text, 'Missing Aspect warning panel; see terminal.txt'
    assert re.findall(r'REWARD_KEY (\d+)',drops(1,0))[-1]=='1'
    energy_drain()
    restore();assert re.findall(r'REWARD_KEY (\d+)',drops(1,0))[-1]=='1'
    energy_drain()
    kill();assert re.findall(r'REWARD_KEY (\d+)',drops(1,1))[-1]=='1'
    restore();assert re.findall(r'REWARD_KEY (\d+)',drops(1,1))[-1]=='1'
    assert not (game.path/'paniclog').exists()
    print('PASS actual Aspect warning, one Third Key, death slab and alive/dead save-reload',flush=True)
finally:
    game.close()
