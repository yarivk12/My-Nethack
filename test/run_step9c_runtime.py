"""Wizard traversal of every packaged Mithardir floor and directed portal.

This runner clears combatants between floors; dedicated combat/service tests
cover encounters. It does not change production connectors or level layouts.
"""
import re
import sys
from run_step8a_runtime import Game

release, output = sys.argv[1:3]
game = Game(release, output)
return_portals = {}

def secure():
    game.lua('nh.debug_flags({hunger=false,mongen=false});nh.debug_flags({mongen=true})')
    # Portal travel applies three turns of dizziness. Wait it out before
    # coordinate-based movement; otherwise a valid route can walk randomly.
    for _ in range(30):
        if not re.search(r'\b(?:Stun|Conf)\b', game.text()):
            break
        game.send('.', 1)
        game.settle()
    assert not re.search(r'\b(?:Stun|Conf)\b', game.text()), game.text()

def feed():
    text = game.lua('''if u.uhunger<500 then
local o=obj.new("uncursed food ration");u.giveobj(o)
nh.pline("FOOD "..o:totable().invlet) end''')
    matches = re.findall(r'FOOD (.)', text)
    if matches:
        game.send('e' + matches[-1], 2)
        game.settle()

def boundary_vision():
    """Exercise top-row vision after the actual branch cleanup lights the map."""
    secure()
    text = game.lua('''local ox,oy=nh.abscoord(0,0)
local spots={}
for x=1,79 do local m=nh.getmap(x-ox,0-oy)
 if string.find(".QsGeI#",m.mapchr,1,true) and not m.has_trap then
  spots[#spots+1]=x
 end
end
assert(#spots>0,"no top-row terrain for vision regression")
nh.pline("TOP_ROW "..spots[1].." "..spots[#spots])''')
    left, right = map(int, re.findall(r'TOP_ROW (\d+) (\d+)', text)[-1])
    for tx, ty in [(left, 0), (right, 0)]:
        feed()
        x, y = game.state()[2:]
        game.send('\x14'); game.wait('Where do you want to be teleported?')
        game.settle()
        game.send(('l' if tx>x else 'h')*abs(tx-x)
                  + ('j' if ty>y else 'k')*abs(ty-y) + '.')
        game.settle()
        assert game.state()[2:] == (tx, ty), game.text()
    assert not (game.path / 'paniclog').exists(), 'top-row vision error'
    print('PASS actual branch top-row vision at both map edges', flush=True)

def portal(side, target):
    feed()
    secure()
    before = game.state()
    # Special levels may flip horizontally. Remember actual arrival portals
    # instead of assuming that the donor's drawn west remains screen-left.
    known = return_portals.get((before[:2], target))
    arrivals = [spot for (level, _), spot in return_portals.items()
                if level == before[:2]]
    excluded = ' or '.join('(q.x==%d and q.y==%d)' % spot for spot in arrivals)
    excluded = excluded or '(q.x==u.ux and q.y==u.uy)'
    choose = ('local p=nil;for _,q in ipairs(ps) do if q.x==%d and q.y==%d then p=q end end'
              % known) if known else '''local p=ps[1]
if #ps==2 then
 for _,q in ipairs(ps) do if not (EXCLUDED) then p=q end end
end'''.replace('EXCLUDED', excluded)
    text = game.lua('''local ps={};local ox,oy=nh.abscoord(0,0)
for x=1,79 do for y=0,20 do
 if nh.getmap(x-ox,y-oy).has_trap then
  local t=nh.gettrap(x-ox,y-oy)
  if t.ttyp_name=="magic portal" then ps[#ps+1]={x=x,y=y} end
 end
end end
assert(#ps>0 and #ps<=2,"wrong portal count")
CHOOSE
assert(p,"remembered arrival portal is missing");local spot=nil;local gate=nil
for _,d in ipairs({{-1,0},{1,0},{0,-1},{0,1},{-1,-1},{1,-1},{-1,1},{1,1}}) do
 local x,y=p.x+d[1],p.y+d[2]
 if x>=1 and x<80 and y>=0 and y<21 then
  local m=nh.getmap(x-ox,y-oy)
  if string.find(".QsGeI#",m.mapchr,1,true) and not m.has_trap then
   spot={x=x,y=y};break
  end
 end
end
if not spot then
 for _,d in ipairs({{-1,0},{1,0},{0,-1},{0,1}}) do
  local x,y=p.x+d[1],p.y+d[2]
  local ax,ay=x+d[1],y+d[2]
  if ax>=1 and ax<80 and ay>=0 and ay<21 then
   local m=nh.getmap(x-ox,y-oy)
   local a=nh.getmap(ax-ox,ay-oy)
   if (m.typ_name=="secret door" or m.typ_name=="door")
      and a.mapchr=="." and not a.has_trap then
    gate={x=x,y=y};spot={x=ax,y=ay};break
   end
  end
 end
end
assert(spot,"no walkable portal approach")
if gate then nh.pline(string.format("PORTAL_GATE %d %d",gate.x,gate.y)) end
nh.pline(string.format("PORTAL_MOVE %d %d %d %d %d %d",u.ux,u.uy,spot.x,spot.y,p.x,p.y))
'''.replace('CHOOSE', choose))
    x, y, tx, ty, px, py = map(int, re.findall(r'PORTAL_MOVE (\d+) (\d+) (\d+) (\d+) (\d+) (\d+)', text)[-1])
    if (x, y) != (tx, ty):
        game.send('\x14')
        game.wait('Where do you want to be teleported?')
        game.settle()
        game.send(('l' if tx>x else 'h')*abs(tx-x)
                  + ('j' if ty>y else 'k')*abs(ty-y) + '.')
        game.settle()
    assert game.state()[2:] == (tx, ty), game.text()
    keys={(-1,-1):'y',(0,-1):'k',(1,-1):'u',(-1,0):'h',(1,0):'l',
          (-1,1):'b',(0,1):'j',(1,1):'n'}
    gates = re.findall(r'PORTAL_GATE (\d+) (\d+)', text)
    if gates:
        gx, gy = map(int, gates[-1])
        direction = keys[gx-tx, gy-ty]
        for _ in range(10):
            state = game.lua('local ox,oy=nh.abscoord(0,0);'
                             'nh.pline("GATE_TYPE "..nh.getmap(%d-ox,%d-oy).typ_name)' % (gx, gy))
            if 'GATE_TYPE secret door' not in state:
                break
            game.send('20s', 1)
            game.settle()
        assert 'GATE_TYPE secret door' not in state, state
        for _ in range(20):
            game.send('o' + direction, 1)
            game.settle()
            game.send(direction, 1)
            game.settle()
            if game.state()[2:] == (gx, gy):
                break
        assert game.state()[2:] == (gx, gy), game.text()
        tx, ty = gx, gy
        print('PASS searched and opened Last Spire portal door', flush=True)
    game.send(keys[px-tx, py-ty], 1)
    game.settle()
    if 'Really step into that magic portal?' in game.text():
        game.send('y', 1)
        game.settle()
    assert game.state()[:2] == target, (target, game.state(), game.text())
    return_portals[(target, before[:2])] = game.state()[2:]
    print('PASS directed portal to', target, flush=True)

def restore():
    global game
    before=game.state()
    game.save();game.close();game=Game(release,output,restore=True)
    assert game.state()==before
    print('PASS traversal save/reload', before[:2], flush=True)

try:
    game.send('#levelchange\n');game.wait('To what experience level')
    game.send('30\n');game.settle()
    game.lua('nh.debug_flags({hunger=false})')
    mith=game.dungeon_number('Mithardir')
    parent=game.branch_depth('Mithardir')
    assert 30<=parent<=199
    game.send('\x16');game.wait('To what level');game.send(str(parent)+'\n',1);game.settle()
    assert game.state()[:2]==(0,parent)
    portal(0,(mith,1));boundary_vision();restore()
    portal(0,(mith,2));portal(1,(mith,3));portal(1,(mith,4));restore()
    secure();assert game.stair('down')[:2]==(mith,5)
    portal(0,(mith,6));portal(1,(mith,7));restore()
    for level in range(8,11):
        secure();assert game.stair('down')[:2]==(mith,level)
        game.lua('''local n=0;local ox,oy=nh.abscoord(0,0)
for x=1,79 do for y=0,20 do
 local m=nh.getmap(x-ox,y-oy)
 assert(m.mapchr~="#" and m.mapchr~="S","unconverted Catacomb corridor/secret door")
 if m.mapchr=="." then n=n+1 end
end end
assert(n>100,"Catacomb generator did not run")
''')
        if level==8:restore()
    restore()
    game.lua('for _,s in ipairs(nh.stairways()) do assert(s.up,"terminal down stair") end')
    for level in range(9,6,-1):
        secure();assert game.stair('up')[:2]==(mith,level)
    portal(0,(mith,4));portal(0,(mith,3));portal(0,(mith,2))
    portal(0,(mith,1));portal(1,(0,parent))
    print('PASS all ten Mithardir floors, Catacombs, directed Spire shortcut and return to DoD%d' % parent, flush=True)
finally:
    game.close()
