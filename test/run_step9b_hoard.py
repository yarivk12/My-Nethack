"""Actual Caves population/hoard, chromatic melee/death and corpse revival."""
from pathlib import Path
import re
import sys
from run_step8a_runtime import Game

release,output=sys.argv[1:3]
game=Game(release,output)
try:
    game.send('#levelchange\n');game.wait('To what experience level');game.send('30\n');game.settle()
    game.lua('nh.debug_flags({hunger=false})')
    caves=game.dungeon_number('The Dragon Caves')
    parent=game.branch_depth('The Dragon Caves')
    assert 30<=parent<=199
    game.send('\x16');game.wait('To what level');game.send(str(parent)+'\n',1);game.settle()
    game.stair('down',True,caves)
    game.lua('nh.debug_flags({mongen=false});nh.debug_flags({mongen=true})')
    def census(label):
        game.send('#wizborn\n',1);text=game.wait('--More--',more=False)
        (game.path/(label+'.txt')).write_text(text,encoding='utf8')
        game.send('\x1b');game.settle()
        return {name.strip():(int(dead),int(born)) for dead,born,name in
                re.findall(r'(?m)^\s*(\d+)\s+(\d+)\s+[EGX ]\s+([A-Za-z][^\n]+?)\s*$',text)}
    before=census('before-hoard')
    source=(Path(__file__).resolve().parents[1]/'dat/drgnD.lua').read_text(encoding='utf8')
    # Fix orientation and sleep bystanders only in this external combat fixture.
    source='''local original=des.monster
des.monster=function(t) t=t or {};t.asleep=true;return original(t) end
'''+source+'''
des.monster=original;des.level_flags("noflip")
local x,y=nh.abscoord(0,0);nh.variable("hoard_origin",{x=x,y=y})
'''
    (game.path/'hoard.lua').write_text(source,encoding='utf8')
    game.send('#wizloaddes\n');game.wait('Load which des lua file?');game.send('hoard.lua\n',2);game.settle()
    after=census('hoard-born')
    delta={name:values[1]-before.get(name,(0,0))[1] for name,values in after.items()}
    assert delta['chromatic cave dragon']==3,delta
    assert sum(n for name,n in delta.items() if name.endswith(' dragon'))==25,delta
    assert sum(n for name,n in delta.items() if 'worm' in name)==6,delta
    assert delta.get('gold dragon',0)==0 and delta.get('Chromatic Dragon',0)==0,delta
    loot='''
local o=obj.next();local gold,gem,tool,weapon,potion,scroll,boulder=0,0,0,0,0,0,0
while not o:isnull() do local t=o:totable()
 if t.oclass=="$" then gold=gold+t.quan
 elseif t.oclass=="*" then gem=gem+t.quan
 elseif t.oclass=="(" then tool=tool+t.quan
 elseif t.oclass==")" then weapon=weapon+t.quan
 elseif t.oclass=="!" then potion=potion+t.quan
 elseif t.oclass=="?" then scroll=scroll+t.quan end
 if t.otyp_name=="boulder" then boulder=boulder+1 end
 o=o:next()
end
assert(gold>=4184 and gem>=17 and tool>=3 and weapon>=3 and potion>=3 and scroll>=3 and boulder>=1,"missing hoard")
nh.pline(string.format("HOARD gold=%d gems=%d tools=%d weapons=%d potions=%d scrolls=%d",gold,gem,tool,weapon,potion,scroll))
'''
    game.lua(loot);game.save();game.close();game=Game(release,output,restore=True);game.lua(loot)
    assert census('restored-hoard')['chromatic cave dragon']==after['chromatic cave dragon']
    print('PASS actual terminal pool: 22 eligible dragons, three chromatics, six worms, hoard and save/restore',flush=True)
    # Isolate one imported dragon for combat, without changing its definition.
    game.lua('''nh.debug_flags({hunger=false,mongen=false});nh.debug_flags({mongen=true})
des.map({x=1,y=0,map="x"});local p=nh.variable("hoard_origin");local ox,oy=nh.abscoord(0,0)
des.monster({id="chromatic cave dragon",coord={p.x+1-ox,p.y+7-oy},asleep=true,peaceful=false})''')
    text=game.lua('local p=nh.variable("hoard_origin");nh.pline(string.format("TARGET %d %d %d %d",u.ux,u.uy,p.x+2,p.y+7))')
    x,y,tx,ty=map(int,re.findall(r'TARGET (\d+) (\d+) (\d+) (\d+)',text)[-1])
    game.send('\x14');game.wait('Where do you want to be teleported?');game.settle()
    game.send(('l' if tx>x else 'h')*abs(tx-x)+('j' if ty>y else 'k')*abs(ty-y)+'.');game.settle()
    assert game.state()[2:]==(tx,ty)
    def hp():
        text=game.lua('nh.pline(string.format("HEALTH %d",u.uhp))')
        return int(re.findall(r'HEALTH (\d+)',text)[-1])
    health=hp();game.send('h');game.settle()
    for _ in range(12):
        if hp()<health:break
        game.send('m.');game.settle()
    assert hp()<health,'chromatic failed to attack'
    def kill():
        game.send('#wizkill\n');game.wait('Pick first monster to slay');game.settle()
        game.send('h.');game.wait('Next monster');game.send('\x1b');game.settle()
    kill()
    corpse='''local o=obj.next();local found=false
while not o:isnull() do local t=o:totable()
if t.otyp_name=="corpse" and t.corpsenm_name=="chromatic cave dragon" then found=true end
o=o:next() end;assert(found,"missing chromatic corpse")'''
    game.lua(corpse);game.save();game.close();game=Game(release,output,restore=True);game.lua(corpse)
    text=game.lua('local o=obj.new("wand of undead turning (0:10)");u.giveobj(o);nh.pline("REVIVE_WAND "..o:totable().invlet)')
    wand=re.findall(r'REVIVE_WAND (.)',text)[-1]
    for cycle in range(2):
        game.send('z'+wand);game.wait('In what direction?');game.send('h');game.settle()
        game.lua(corpse.replace('assert(found,','assert(not found,'))
        game.send('#wizborn\n');game.wait('chromatic cave dragon',more=False);game.send('\x1b');game.settle()
        kill();game.lua(corpse)
    print('PASS actual chromatic melee damage, death/corpse save/restore and two native wand revivals/deaths',flush=True)
finally:
    game.close()
