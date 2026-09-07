"""Kill the real bridge boss, check its gear and unique count, save/reload.

Wizard slaying is controlled validation, not a balance/combat simulation.
"""
import re
import sys
from run_step8a_runtime import Game

release,output=sys.argv[1:3]
game=Game(release,output)
def census(game):
    game.send('#wizborn\n')
    text=game.wait("Durin's Bane",more=False)
    (game.path/'boss-census.txt').write_text(text,encoding='utf8')
    assert re.search(r'1\s+1\s+E\s+Durin\'s Bane',text),text
    game.send('\x1b');game.settle()

try:
    game.send('#levelchange\n');game.wait('To what experience level');game.send('30\n');game.settle()
    game.lua('nh.debug_flags({hunger=false})')
    moria_depth=game.branch_depth('The Ruins of Moria')
    game.send('\x16');game.wait('To what level');game.send(str(moria_depth)+'\n',1);game.settle()
    game.stair('up',True);game.stair('up')
    _,_,x,y=game.state()
    # The awake boss can move during entry. Read its actual TTY location;
    # do not target the map's original spawn coordinate after turns pass.
    lines=game.text().splitlines()[1:23]
    hero=[(xx,yy) for yy,line in enumerate(lines) for xx,c in enumerate(line) if c=='@']
    bosses=[(xx,yy) for yy,line in enumerate(lines) for xx,c in enumerate(line) if c=='&']
    assert len(hero)==len(bosses)==1,game.text()
    tx=x+bosses[0][0]-hero[0][0];ty=y+bosses[0][1]-hero[0][1]
    game.send('#wizkill\n');game.wait('Pick first monster to slay')
    game.settle()
    game.send(('l' if tx>x else 'h')*abs(tx-x)+('j' if ty>y else 'k')*abs(ty-y)+'.')
    game.wait('Next monster');game.send('\x1b');game.settle()
    census(game)
    check='''
local o=obj.next();local whip,wand,potion=false,false,false
while not o:isnull() do
 local t=o:totable()
 if t.otyp_name=="bullwhip" then assert(t.spe>=1 and t.spe<=7 and t.cursed==0);whip=true end
 if t.otyp_name=="speed monster" then wand=true end
 if t.otyp_name=="paralysis" then assert(t.cursed~=0);potion=true end
 o=o:next()
end
assert(whip and wand and potion,"missing boss equipment drops")
'''
    game.lua(check)
    before=game.state();game.save();game=Game(release,output,restore=True)
    assert game.state()==before
    census(game);game.lua(check)
    game.stair('down');game.stair('down',True)
    print("PASS actual Durin's Bane death, unique birth/death count, gear drops, save/reload and return to DoD%d" % moria_depth,flush=True)
finally:
    game.close()
