"""Actual worn living/barnacle armor tentacles before and after save/restore."""
import re,sys
from run_step8a_runtime import Game as BaseGame
class Game(BaseGame):
    def send(self,keys,delay=.3):
        if not hasattr(self,'observations'):self.observations=[]
        self.observations.append(self.text());super().send(keys,delay)
        self.observations.append(self.text())
release,output=sys.argv[1:3]
armor=sys.argv[3] if len(sys.argv)>3 else 'living armor'
assert armor in ('living armor','barnacle armor')
game=Game(release,output)
try:
    game.lua('nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()')
    game.send('#levelchange\n');game.wait('To what experience level')
    game.send('30\n');game.settle()
    (game.path/'living.lua').write_text('''
des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip")
des.map({x=1,y=0,map=[=[
-------------
|...........|
|...........|
|...........|
|...........|
|...........|
-------------
]=]})
des.region(selection.area(1,1,11,5),"lit")
''',encoding='utf8')
    game.send('#wizloaddes\n');game.wait('Load which des lua file?')
    game.send('living.lua\n',2);game.settle()
    x,y=game.state()[2:]
    game.send('\x14');game.wait('Where do you want to be teleported?');game.settle()
    game.send(('l' if 4>x else 'h')*abs(4-x)+('j' if 3>y else 'k')*abs(3-y)+'.')
    game.settle();assert game.state()[2:]==(4,3)
    text=game.lua('local o=obj.new("uncursed +0 '+armor+'");'
                  'assert(o:totable().otyp_name=="'+armor+'","wrong living armor identity");u.giveobj(o);'
                  'nh.pline("ARMOR_LETTER "..o:totable().invlet)')
    letter=re.findall(r'ARMOR_LETTER (.)',text)[-1]
    game.send('W'+letter);game.settle()
    game.lua('nh.debug_flags({mongen=true});'
             'des.monster({id="iron golem",coord={4,3},peaceful=false,paralyzed=126})')

    def attack(label):
        start=len(game.observations)
        for _ in range(16):
            game.send('.');game.settle()
            text='\n'.join(game.observations[start:])
            if "armor's tentacles lash" in text:break
        assert "armor's tentacles lash" in text, 'No living armor attack; see terminal.txt'
        print('PASS actual worn '+armor+' tentacle attack '+label,flush=True)

    attack('before save')
    game.save();game.close();game=Game(release,output,restore=True)
    attack('after restore')
    assert not (game.path/'paniclog').exists()
finally:
    game.close()
