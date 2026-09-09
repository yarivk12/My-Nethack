"""Actual awake First Wraithworm reaches through five squares after restore.

Iron bars hold the hero against its wind without suppressing monster AI.
"""
import re,sys
from run_step8a_runtime import Game as BaseGame
class Game(BaseGame):
    def text(self):
        value=super().text()
        if not hasattr(self,'observations'):self.observations=[]
        if not self.observations or self.observations[-1]!=value:self.observations.append(value)
        return value
release,output=sys.argv[1:3]
game=Game(release,output)
try:
    game.lua('nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()')
    game.send('#levelchange\n');game.wait('To what experience level');game.send('30\n');game.settle()
    (game.path/'worm-bite.lua').write_text('''des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip")
des.map({x=1,y=0,map=[=[
---------------------
|...................|
|...................|
|..FFF..............|
|..F.F..............|
|..FFF..............|
|...................|
|...................|
---------------------
]=]})
des.region(selection.area(1,1,19,7),"lit")
''',encoding='utf8')
    game.send('#wizloaddes\n');game.wait('Load which des lua file?');game.send('worm-bite.lua\n',2);game.settle()
    x,y=game.state()[2:]
    game.send('\x14');game.wait('Where do you want to be teleported?');game.settle()
    game.send(('l' if 5>x else 'h')*abs(5-x)+('j' if 4>y else 'k')*abs(4-y)+'.');game.settle()
    assert game.state()[2:]==(5,4)
    game.lua('nh.debug_flags({mongen=true});des.monster({id="first wraithworm",coord={9,4},peaceful=false})')
    for phase in range(2):
        start=len(game.observations)
        for _ in range(25):
            game.send('.',.4);game.settle()
            text='\n'.join(game.observations[start:])
            if re.search(r'wraithworm bites',text,re.I):break
        assert re.search(r'wraithworm bites',text,re.I),text
        assert game.state()[2:]==(5,4),'Wind fixture did not hold the five-square distance'
        print('PASS actual awake First Wraithworm bite at range five, phase',phase,flush=True)
        game.save();game.close();game=Game(release,output,restore=True)
finally:
    game.close()
