"""Production forge Lua/map placement, movement, apply, save and recovery."""
from pathlib import Path
import os,re,subprocess,sys
from run_step8a_runtime import Game
release,output=sys.argv[1:3]
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
 g.send('a'+letter);g.wait('no forge operations');g.settle();g.lua(check)
 # Real departure and return go through the ordinary level-file machinery.
 for target in (2,1):
  g.send('\x16');g.wait('To what level');g.send(str(target)+'\n',1);g.settle()
 g.lua(check)
 g.save();g=Game(release,output,restore=True);g.lua(check)
finally:g.close()
r=subprocess.run([str(Path(release).resolve()/'recover.exe'),'-d',str(Path(output).resolve()),'wizard'],capture_output=True,text=True)
(Path(output)/'recovery.log').write_text(r.stdout+r.stderr)
assert r.returncode==0,(r.returncode,r.stdout,r.stderr)
g=Game(release,output,restore=True)
try:
 g.lua(check)
 print('PASS production map Lua, walk/apply, level transition, full save/restore and recover.exe forge checkpoint')
finally:g.close()
