"""Production forge Lua/map placement, movement, apply, save and recovery."""
from pathlib import Path
import hashlib,os,re,shutil,subprocess,sys
from run_step8a_runtime import Game
release,output=sys.argv[1:3]
# Optional current-format diagnostic save supplies an enhanced ingredient.
# All subsequent commands run the byte-identical production executable.
if len(sys.argv)>3:
 os.environ.update(STEP13_GAME_FIXTURE='1',STEP15C_GAME_SEED='1')
 seed=Game(sys.argv[3],output)
 try: seed.save()
 finally:
  seed.close();os.environ.pop('STEP13_GAME_FIXTURE',None);os.environ.pop('STEP15C_GAME_SEED',None)
 shutil.copy2(Path(release)/'NetHack.exe',Path(output)/'NetHack.exe')
 assert hashlib.sha256((Path(output)/'NetHack.exe').read_bytes()).digest()==hashlib.sha256((Path(release)/'NetHack.exe').read_bytes()).digest()
 g=Game(release,output,restore=True)
else:
 g=Game(release,output)
check='''local p=nh.variable("forge_pos");local ox,oy=nh.abscoord(0,0)
local m=nh.getmap(p.x-ox,p.y-oy)
assert(m.typ_name=="forge" and m.mapchr=="f" and m.lit,"forge persistence")'''
try:
 g.lua('nh.debug_flags({hunger=false,mongen=false})')
 (g.path/'forge.lua').write_text('''
des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip", "mazelevel")
des.map([[
-------
|.f...|
|.....|
-------
]])
des.region(selection.area(1,1,5,2),"unlit")
des.stair("up",1,1)
des.stair("down",5,2)
local x,y=nh.abscoord(2,1);nh.variable("forge_pos",{x=x,y=y})
''')
 g.send('#WIZLOADDES\n');g.wait('Load which des lua file?');g.send('FORGE.LUA\n',2);g.settle()
 g.lua(check)
 # Place hero on a real ROOM then walk onto the adjacent forge.
 t=g.lua('local p=nh.variable("forge_pos");nh.pline(string.format("TARGET %d %d %d %d",u.ux,u.uy,p.x-1,p.y))')
 a,b,tx,ty=map(int,re.findall(r'TARGET (\d+) (\d+) (\d+) (\d+)',t)[-1])
 if (a,b)!=(tx,ty):
  g.send('\x14');g.wait('Where do you want to be teleported?');g.settle()
  g.send(('l' if tx>a else 'h')*abs(tx-a)+('j' if ty>b else 'k')*abs(ty-b)+'.');g.settle()
 g.send('l');g.settle()
 g.lua('local p=nh.variable("forge_pos");assert(u.ux==p.x and u.uy==p.y,"walkable forge")')
 t=g.lua('local o=obj.new("uncursed war hammer");u.giveobj(o);nh.pline("HAMMER "..o:totable().invlet)')
 letter=re.findall(r'HAMMER (.)',t)[-1]
 g.lua('nh.variable("forge_turn",u.moves)')
 g.send('a'+letter);g.wait('Forge an item');g.send('\x1b');g.settle();g.lua(check)
 g.lua('assert(u.moves==nh.variable("forge_turn"),"cancel costs no turn")')
 t=g.lua('''
 local a=obj.new("uncursed +4 long sword named forge-left");u.giveobj(a)
 local b=obj.new("cursed -3 long sword named forge-right");u.giveobj(b)
 nh.pline("INGREDIENTS "..a:totable().invlet.." "..b:totable().invlet)
 nh.variable("forge_turn",u.moves)
 ''')
 first,second=re.findall(r'INGREDIENTS (.) (.)',t)[-1]
 # Lua giveobj bypasses ordinary pickup observation. View the inventory so
 # the native dknown gate is satisfied, as it would be after visible pickup.
 g.send('i');g.wait('forge-left');g.send('\x1b');g.settle()
 g.send('a'+letter);g.wait('Forge an item');g.send('f');g.wait('Choose a category')
 g.send('a');g.wait('katana - 2 long swords');g.send('a');g.wait('Choose 1 long sword')
 g.send(first);g.settle();g.send(second);g.wait('Confirm crafting?')
 assert 'forge-left' in g.text() and 'forge-right' in g.text(),g.text()
 g.send('y');g.wait('You forge:');g.settle()
 g.lua('''
 assert(u.moves==nh.variable("forge_turn")+1,"craft costs one turn")
 local o=u.inventory;local swords,outputs=0,0
 while not o:isnull() do
   local t=o:totable()
   if t.otyp_name=="long sword" then swords=swords+t.quan end
   if t.otyp_name=="katana" then
     outputs=outputs+t.quan
     assert(t.spe==4 and t.cursed==0 and t.blessed==0 and t.known==0 and t.bknown==0,"inherited output")
   end
   o=o:next()
 end
 assert(swords==0 and outputs==1,"exact consumption and single inventory output")
 ''')
 # A real second craft consumes the first output; selection stays explicit.
 t=g.lua('''local o=obj.new("uncursed two-handed sword named chain-input");u.giveobj(o)
 nh.pline("CHAIN "..o:totable().invlet)
 local p=u.inventory;while not p:isnull() do local t=p:totable()
 if t.otyp_name=="katana" then nh.pline("KATANA "..t.invlet) end;p=p:next() end
 nh.variable("forge_turn",u.moves)''')
 katana=re.findall(r'KATANA (.)',t)[-1];chain=re.findall(r'CHAIN (.)',t)[-1]
 g.send('i');g.wait('chain-input');g.send('\x1b');g.settle()
 g.send('a'+letter);g.wait('Forge an item');g.send('f');g.wait('Choose a category')
 g.send('a');g.wait('Weapons');g.send('a');g.wait('Choose 1 katana');g.send(katana)
 g.wait('Choose 1 two-handed sword');g.send(chain);g.wait('Confirm crafting?');g.send('y')
 g.wait('You forge:');g.settle()
 g.lua('''assert(u.moves==nh.variable("forge_turn")+1,"reforging one turn")
 local p=u.inventory;local found=0;while not p:isnull() do local t=p:totable()
 assert(t.otyp_name~="katana" and t.otyp_name~="two-handed sword","reforging consumed both")
 if t.otyp_name=="tsurugi" then found=found+t.quan;assert(t.spe==4 and t.bknown==0) end
 p=p:next() end;assert(found==1,"one reforged output")''')
 if len(sys.argv)>3:
  t=g.lua('''local o=obj.new("cursed rusty corroded -2 stiletto named enhanced-partner");u.giveobj(o)
  nh.pline("PARTNER "..o:totable().invlet)
  local p=u.inventory;while not p:isnull() do local t=p:totable()
  if t.oname=="step13-inventory" then nh.pline("ENHANCED "..t.invlet) end;p=p:next() end
  nh.variable("forge_turn",u.moves)''')
  enhanced=re.findall(r'ENHANCED (.)',t)[-1];partner=re.findall(r'PARTNER (.)',t)[-1]
  g.send('i');g.wait('enhanced-partner');g.send('\x1b');g.settle()
  g.send('a'+letter);g.wait('Forge an item');g.send('f');g.wait('Choose a category')
  g.send('a');g.wait('Weapons');g.send('a');g.wait('Choose 1 dagger');g.send(enhanced)
  g.wait('Choose 1 stiletto');g.send(partner);g.wait('Confirm crafting?')
  assert 'step13-inventory' in g.text(),g.text()
  g.send('y');g.wait('You forge:');g.settle()
  g.lua('''assert(u.moves==nh.variable("forge_turn")+1,"enhanced ingredient one turn")
  local p=u.inventory;local found=0;while not p:isnull() do local t=p:totable()
  assert(t.oname~="step13-inventory","enhanced input consumed")
  if t.otyp_name=="athame" then found=found+t.quan;assert(t.spe==-2 and t.cursed==1 and t.bknown==0 and t.known==0) end
  p=p:next() end;assert(found==1,"inherited athame output")''')
 # Persist actual and knowledge checks through every later production stage.
 # Lua exposes native fields; the diagnostic restore below verifies the four
 # enhancement fields without identifying or changing the production items.
 check+='''
 local p=u.inventory;local found=0;while not p:isnull() do local t=p:totable()
 if t.otyp_name=="tsurugi" or t.otyp_name=="athame" then
   found=found+1
   assert(t.spe==(t.otyp_name=="tsurugi" and 4 or -2),"inherited spe persists")
   assert(t.known==0 and t.bknown==0 and t.rknown==0 and t.dknown==1,"normal knowledge persists")
   assert(t.cursed==(t.otyp_name=="athame" and 1 or 0) and t.blessed==0 and t.greased==0 and t.has_oname==0,"allowlist")
   assert(t.oeroded==(t.otyp_name=="athame" and 1 or 0) and t.oeroded2==t.oeroded,"erosion persists")
   assert(t.oerodeproof==(t.otyp_name=="athame" and 1 or 0),"proofing persists")
 end;p=p:next() end
 assert(found=='''+('2' if len(sys.argv)>3 else '1')+''',"forged items persist")'''
 g.lua(check)
 g.save()
 if len(sys.argv)>3: shutil.copytree(output,output+'-after-craft')
 g=Game(release,output,restore=True)
 # Real departure and return go through the ordinary level-file machinery.
 for target in (2,1):
  g.send('\x16');g.wait('To what level');g.send(str(target)+'\n',1);g.settle()
 g.lua(check)
 g.save()
 if len(sys.argv)>3: shutil.copytree(output,output+'-after-level')
 g=Game(release,output,restore=True);g.lua(check)
finally:g.close()
r=subprocess.run([str(Path(release).resolve()/'recover.exe'),'-d',str(Path(output).resolve()),'wizard'],capture_output=True,text=True)
(Path(output)/'recovery.log').write_text(r.stdout+r.stderr)
assert r.returncode==0,(r.returncode,r.stdout,r.stderr)
if len(sys.argv)>3: shutil.copytree(output,output+'-after-recovery')
g=Game(release,output,restore=True)
try:
 g.lua(check)
 print('PASS production forge menus, exact named ingredients, cancellation/one-turn crafting, two-craft chain, level transition, save/restore and recover.exe checkpoint')
 if len(sys.argv)>3: print('PASS production crafting inherits enhanced ingredient from explicitly diagnostic-created current-format save')
finally:g.close()

if len(sys.argv)>3:
 # Inspect separate copies using the current-format native restore path. The
 # gameplay above exclusively used the byte-identical production executable.
 os.environ.update(STEP13_GAME_FIXTURE='1',STEP15C_GAME_CHECK='1')
 try:
  for stage in ('after-craft','after-level','after-recovery'):
   probe=output+'-'+stage
   shutil.copy2(Path(sys.argv[3])/'NetHack.exe',Path(probe)/'NetHack.exe')
   g=Game(sys.argv[3],probe,restore=True)
   try:
    result=(Path(probe)/'step15c-game-results.txt').read_text()
    assert 'PASS production forged' in result
    print('PASS Step 15C diagnostic inspection of production save '+stage)
   finally:g.close()
 finally:
  os.environ.pop('STEP13_GAME_FIXTURE',None);os.environ.pop('STEP15C_GAME_CHECK',None)
