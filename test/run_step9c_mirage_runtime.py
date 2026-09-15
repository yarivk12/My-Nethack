"""Living mirage disguise, native shape protection and wizard sanity checks."""
import re,sys
from run_step8a_runtime import Game
release,output=sys.argv[1:3]
game=Game(release,output)
def sanity():
    game.lua('nh.parse_config("OPTIONS=sanity_check")')
    game.send('.');game.settle()
    assert not (game.path/'paniclog').exists()
def glyph(x,y,expected):
    game.send('\x12',1);game.settle()
    assert game.screen.display[y+1][x-1]==expected,game.text()
try:
    game.lua('nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()')
    (game.path/'mirage.lua').write_text('''des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip")
des.map({x=1,y=0,map=[=[
-----------------
|...............|
|...............|
|...............|
|...............|
|...............|
|...............|
-----------------
]=]})
des.region(selection.area(1,1,15,6),"lit")
''',encoding='utf8')
    game.send('#wizloaddes\n');game.wait('Load which des lua file?');game.send('mirage.lua\n',2);game.settle()
    x,y=game.state()[2:]
    game.send('\x14');game.wait('Where do you want to be teleported?');game.settle()
    game.send(('l' if 4>x else 'h')*abs(4-x)+('j' if 3>y else 'k')*abs(3-y)+'.');game.settle()
    game.lua('nh.debug_flags({mongen=true});des.monster({id="living mirage",coord={9,3},peaceful=false,paralyzed=126})')
    glyph(10,3,'}');sanity()
    game.save();game.close();game=Game(release,output,restore=True)
    glyph(10,3,'}');sanity()
    text=game.lua('local o=obj.new("uncursed ring of protection from shape changers");u.giveobj(o);nh.pline("RING "..o:totable().invlet)')
    ring=re.findall(r'RING (.)',text)[-1]
    game.send('P'+ring);game.settle()
    if 'Which ring-finger' in game.text():game.send('l');game.settle()
    glyph(10,3,'P');sanity()
    game.lua('des.monster({id="living mirage",coord={9,5},peaceful=false,paralyzed=126})')
    glyph(10,5,'P');sanity()
    game.save();game.close();game=Game(release,output,restore=True)
    glyph(10,3,'P');glyph(10,5,'P');sanity()
    print('PASS puddle disguise and save, existing/new mirages revealed under shape protection, restore and native wizard sanity',flush=True)
finally:
    game.close()
