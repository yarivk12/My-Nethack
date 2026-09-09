"""Wizard conflict fixture observes elder magic affecting another monster."""
import re
import sys
from run_step8a_runtime import Game

class ObservedGame(Game):
    def __init__(self,*args,**kwargs):
        self.observed=[]
        super().__init__(*args,**kwargs)
    def send(self,keys,delay=.3):
        if hasattr(self,'screen'):self.observed.append(self.text())
        super().send(keys,delay)
        if hasattr(self,'screen'):self.observed.append(self.text())

release,output=sys.argv[1:3]
caster=sys.argv[3] if len(sys.argv)>3 else 'Alabaster elf-elder'
assert caster in ('Alabaster elf-elder','Alabaster mummy')
evidence=r'Scales cover (?:the )?iron golem.s eyes|iron golem seems confused'
if caster=='Alabaster mummy':
    evidence=r'A psychic bolt strikes (?:the )?iron golem|iron golem reels|iron golem suddenly seems weaker'
game=ObservedGame(release,output)
try:
    game.send('#levelchange\n');game.wait('To what experience level')
    game.send('30\n');game.settle()
    game.lua('nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()')
    text=game.lua('local o=obj.new("uncursed ring of conflict");u.giveobj(o);'
                  'nh.pline("CONFLICT_RING "..o:totable().invlet)')
    ring=re.findall(r'CONFLICT_RING (.)',text)[-1]
    game.send('P'+ring);game.settle()
    if 'Which ring-finger' in game.text():game.send('l');game.settle()
    game.lua('nh.debug_flags({mongen=true})')
    (game.path/'elder-mm.lua').write_text('''
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
for x=9,12 do
 des.terrain({coord={x,2},typ="|"})
 des.terrain({coord={x,4},typ="|"})
end
des.terrain({coord={9,3},typ="F"})
des.terrain({coord={12,3},typ="|"})
des.monster({id="CASTER",coord={10,3},peaceful=true,
 keep_default_invent=false,inventory=function()
  des.object({id="gray dragon scale mail",spe=20,buc="uncursed"})
 end})
des.monster({id="iron golem",coord={11,3},peaceful=false,paralyzed=126,
 keep_default_invent=false})
'''.replace('CASTER',caster),encoding='utf8')
    game.send('#wizloaddes\n');game.wait('Load which des lua file?')
    game.send('elder-mm.lua\n',2);game.settle()
    x,y=game.state()[2:]
    game.send('\x14');game.wait('Where do you want to be teleported?');game.settle()
    game.send(('l' if 7>x else 'h')*abs(7-x)+('j' if 3>y else 'k')*abs(3-y)+'.')
    game.settle()
    print('%s conflict fixture ready'%caster,flush=True)
    for turn in range(96):
        text='\n'.join(game.observed)+game.text()
        if re.search(evidence,text,re.I):
            break
        game.send('m.');game.settle()
        if turn%16==15:print('Observed %d conflict turns'%(turn+1),flush=True)
    text='\n'.join(game.observed)+game.text()
    (game.path/'elder-mm-spells.txt').write_text(text,encoding='utf8')
    assert re.search(evidence,text,re.I), \
        'No actual monster-target spell; see elder-mm-spells.txt'
    game.save();game.close();game=ObservedGame(release,output,restore=True)
    print('PASS actual %s spell affects another monster under conflict; save/restore succeeds'%caster,flush=True)
finally:
    game.close()
