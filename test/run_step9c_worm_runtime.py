"""Native First Wraithworm wind radius and encounter persistence."""
import re,sys
from run_step8a_runtime import Game as BaseGame
class Game(BaseGame):
    def text(self):
        text=super().text()
        if not hasattr(self,'observations'):self.observations=[]
        if not self.observations or self.observations[-1]!=text:self.observations.append(text)
        return text
release,output=sys.argv[1:3]
game=Game(release,output)
def teleport(tx,ty):
    x,y=game.state()[2:]
    game.send('\x14');game.wait('Where do you want to be teleported?');game.settle()
    game.send(('l' if tx>x else 'h')*abs(tx-x)+('j' if ty>y else 'k')*abs(ty-y)+'.')
    game.settle()
try:
    game.lua('nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()')
    game.send('#levelchange\n');game.wait('To what experience level');game.send('30\n');game.settle()
    (game.path/'worm.lua').write_text('''des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip")
des.map({x=1,y=0,map=[=[
---------------------
|...................|
|...................|
|...................|
|...................|
|...................|
|...................|
|...................|
---------------------
]=]})
des.region(selection.area(1,1,19,7),"lit")
''',encoding='utf8')
    game.send('#wizloaddes\n');game.wait('Load which des lua file?');game.send('worm.lua\n',2);game.settle()
    teleport(4,4)
    game.lua('nh.debug_flags({mongen=true});des.monster({id="first wraithworm",coord={9,4},peaceful=false,paralyzed=126})')
    start=len(game.observations);game.send('.');game.settle()
    assert 'thrown by a blast of wind' not in '\n'.join(game.observations[start:])
    game.save();game.close();game=Game(release,output,restore=True)
    start=len(game.observations)
    teleport(5,4);game.send('.');game.settle()
    assert 'thrown by a blast of wind' in '\n'.join(game.observations[start:])
    print('PASS actual First Wraithworm: no wind at six squares, wind at five after restore',flush=True)
    game.save();game.close();game=Game(release,output,restore=True)
    assert not (game.path/'paniclog').exists()
finally:
    game.close()
