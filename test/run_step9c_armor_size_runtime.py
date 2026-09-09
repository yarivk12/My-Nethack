"""Native hero wear command applies imported size metadata and exceptions."""
import re,sys
from run_step8a_runtime import Game
release,output=sys.argv[1:3]
game=Game(release,output)
def worn(letter):
    text=game.lua('''local o=u.inventory;while not o:isnull() do local t=o:totable()
if t.invlet=="%s" then nh.pline("WORN "..t.owornmask) end;o=o:next() end'''%letter)
    return int(re.findall(r'WORN (\d+)',text)[-1])
try:
    game.lua('nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()')
    for name,size,accepted in [('jacket','small',False),('jacket','large',False),
                          ('jacket',None,True),('cloak of protection','large',True),
                          ('cornuthaum','small',True),('high-elven helm','small',False)]:
        game.lua('u.clear_inventory()')
        # Imported sizes are level-data metadata, not wish-parser prefixes.
        text=game.lua('''local ox,oy=nh.abscoord(0,0)
local x,y=u.ux-ox,u.uy-oy
des.object({id="%s",%s buc="uncursed",spe=0,coord={x,y}})
local o=obj.at(x,y);assert(o:totable().otyp_name=="%s")
u.giveobj(o);nh.pline("ARMOR "..o:totable().invlet)''' %
                      (name,('size="'+size+'",') if size else '',name))
        letter=re.findall(r'ARMOR (.)',text)[-1]
        game.send('W'+letter,2);game.settle()
        assert bool(worn(letter))==accepted,(name,game.text())
        if accepted:
            game.save();game.close();game=Game(release,output,restore=True)
            assert worn(letter)
            game.send('T'+letter,2);game.settle();assert not worn(letter)
        print('PASS native sized armor wear/restore:',size,name,accepted,flush=True)
finally:
    game.close()
