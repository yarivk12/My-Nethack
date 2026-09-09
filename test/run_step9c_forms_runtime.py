"""Natural full-health Eladrin return transitions after saving alternate forms."""
import sys
from run_step8a_runtime import Game as BaseGame
class Game(BaseGame):
    def text(self):
        text=super().text()
        if not hasattr(self,'observations'):self.observations=[]
        if not self.observations or self.observations[-1]!=text:
            self.observations.append(text)
        return text
release,output=sys.argv[1:3]
game=Game(release,output)
try:
    game.lua('nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()')
    (game.path/'forms.lua').write_text('''
des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip")
des.map({x=1,y=0,map=[=[
-------------------
|.................|
|.................|
|.................|
|.................|
|.................|
|.................|
-------------------
]=]})
des.region(selection.area(1,1,17,6),"lit")
for _,x in ipairs({3,8,13}) do
 des.terrain(selection.area(x-1,1,x+1,3),"F")
 des.terrain(x,2,".")
end
des.terrain(8,2,"}")
''',encoding='utf8')
    game.send('#wizloaddes\n');game.wait('Load which des lua file?')
    game.send('forms.lua\n',2);game.settle()
    x,y=game.state()[2:]
    game.send('\x14');game.wait('Where do you want to be teleported?');game.settle()
    game.send(('l' if 9>x else 'h')*abs(9-x)+('j' if 5>y else 'k')*abs(5-y)+'.')
    game.settle();assert game.state()[2:]==(9,5)
    game.lua('''nh.debug_flags({mongen=true});
des.monster({id="mote of light",coord={3,2},peaceful=true,paralyzed=126})
des.monster({id="water dolphin",coord={8,2},peaceful=true,paralyzed=126})
des.monster({id="singing sand",coord={13,2},peaceful=true,paralyzed=126})''')
    game.save();game.close();game=Game(release,output,restore=True)
    start=len(game.observations)
    game.send('.');game.settle()
    text='\n'.join(game.observations[start:])
    for form in ('Coure Eladrin','Noviere Eladrin','Bralani Eladrin'):
        assert 'changes into a '+form in text, 'Missing natural '+form+' return; see terminal.txt'
    game.save();game.close();game=Game(release,output,restore=True)
    game.send('.');game.settle()
    print('PASS all three full-health elemental return transitions after restore; humanoid state resaved/restored',flush=True)
finally:
    game.close()
