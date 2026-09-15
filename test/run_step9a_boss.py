"""Actual palace combat/rewards with sleeping bystanders in a wizard fixture.

Wizard slaying exercises real death/drop/unique state, separately from combat.
"""
from pathlib import Path
import re
import sys
from run_step8a_runtime import Game

release,output=sys.argv[1:3]
game=Game(release,output)
try:
    game.send('#levelchange\n');game.wait('To what experience level')
    game.send('30\n');game.settle()
    source=(Path(__file__).resolve().parents[1]/'dat/palace_e.lua').read_text()
    source='''local original=des.monster
des.monster=function(t) t=t or {};t.asleep=true;return original(t) end
'''+source+'''
des.monster=original
local x,y=nh.abscoord(61,9);nh.variable("executioner_origin",{x=x,y=y})
'''
    (game.path/'boss.lua').write_text(source)
    game.lua('nh.debug_flags({hunger=false})')
    game.send('#wizloaddes\n');game.wait('Load which des lua file?')
    game.send('boss.lua\n',2);game.settle()
    def census(dead):
        game.send('#wizborn\n');text=game.wait('Executioner',more=False)
        assert re.search(r'\b%d\s+1\s+E\s+Executioner'%dead,text),text
        (game.path/('executioner-census-%d.txt'%dead)).write_text(text,encoding='utf8')
        game.send('\x1b');game.settle()
    census(0)
    game.save();game.close();game=Game(release,output,restore=True)
    game.lua('nh.debug_flags({hunger=false})');census(0)
    text=game.lua('local p=nh.variable("executioner_origin");nh.pline(string.format("BOSS %d %d %d %d",u.ux,u.uy,p.x,p.y))')
    x,y,tx,ty=map(int,re.findall(r'BOSS (\d+) (\d+) (\d+) (\d+)',text)[-1])
    game.send('\x14');game.wait('Where do you want to be teleported?');game.settle()
    game.send(('l' if tx-1>x else 'h')*abs(tx-1-x)+('j' if ty>y else 'k')*abs(ty-y)+'.')
    game.settle()
    assert game.state()[2:]==(tx-1,ty)
    def hp():
        text=game.lua('nh.pline(string.format("BOSS_TEST_HP %d",u.uhp))')
        return int(re.findall(r'BOSS_TEST_HP (\d+)',text)[-1])
    health=hp()
    game.send('l');game.settle()  # wake the real boss with a melee attack
    for _ in range(12):
        if hp()<health:break
        game.send('m.');game.settle()
    assert hp()<health,'Executioner failed to attack in actual melee'
    print('PASS actual Executioner melee damages the hero',flush=True)
    x,y=game.state()[2:]
    game.send('#wizkill\n');game.wait('Pick first monster to slay');game.settle()
    game.send(('l' if tx>x else 'h')*abs(tx-x)+('j' if ty>y else 'k')*abs(ty-y)+'.')
    game.wait('Next monster');game.send('\x1b');game.settle();census(1)
    check='''
local axe,cloak,picks,marker,chests=false,false,0,0,0
local function inspect(o)
 while not o:isnull() do
  local t=o:totable()
  if t.otyp_name=="battle-axe" then
   assert(t.blessed~=0 and t.oerodeproof~=0 and t.spe>=0 and t.spe<=5)
   axe=true
  elseif t.otyp_name=="cloak of magic resistance" then cloak=true
  elseif t.otyp_name=="crystal pick" then picks=picks+t.quan
  elseif t.otyp_name=="magic marker" then marker=marker+t.quan
  elseif t.otyp_name=="chest" then chests=chests+1 end
  if t.has_contents~=0 then inspect(o:contents()) end
  o=o:next()
 end
end
inspect(obj.next())
assert(axe and cloak,"missing Executioner equipment drops")
assert(picks>=2 and marker>=1 and chests>=7,"missing palace treasure")
'''
    game.lua(check)
    game.save();game.close();game=Game(release,output,restore=True)
    census(1);game.lua(check)
    print('PASS Executioner unique birth, alive save/restore, actual death/equipment drops, seven chests/two crystal picks/marker, dead save/restore',flush=True)
finally:
    game.close()
