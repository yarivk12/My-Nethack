"""Actual iron-golem rust trap: native control, pronounced Vaul, save/reload."""
import re
import sys
from run_step8a_runtime import Game

release, output = sys.argv[1:3]
game = Game(release, output)
try:
    game.lua('nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()')
    (game.path/'vaul.lua').write_text('''
des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip")
des.map({x=1,y=0,map=[=[
-------------
|...........|
|...........|
|...........|
|...........|
|...........|
-------------
]=]})
des.region(selection.area(1,1,11,5),"lit")
des.trap({type="rust",coord={4,3}})
''', encoding='utf8')
    game.send('#wizloaddes\n'); game.wait('Load which des lua file?')
    game.send('vaul.lua\n',2); game.settle()

    def position():
        x,y=game.state()[2:]
        game.send('\x14'); game.wait('Where do you want to be teleported?'); game.settle()
        game.send(('l' if 4>x else 'h')*abs(4-x)+('j' if 3>y else 'k')*abs(3-y)+'.')
        game.settle(); assert game.state()[2:]==(4,3)

    def iron():
        game.send('#polyself\n'); game.wait('Become what kind of monster?')
        game.send('iron golem\n'); game.settle()
        game.lua('assert(nh.int_to_pmname(u.umonnum)=="iron golem")')

    def trigger():
        # Native 'm' explicitly steps onto a known trap without its prompt.
        game.send('ml'); game.settle()
        assert game.state()[2:]==(5,3), 'Rust fixture movement failed; see terminal.txt'

    position(); iron(); trigger()
    game.lua('assert(nh.int_to_pmname(u.umonnum)~="iron golem","native rust control did not destroy iron form")')
    print('PASS native rust trap destroys unprotected iron-golem form',flush=True)
    position(); iron()
    text=game.lua('local o=obj.new("cursed syllable of spirit: Vaul");u.giveobj(o);'
                  'nh.pline("VAUL_TILE "..o:totable().invlet)')
    tile=re.findall(r'VAUL_TILE (.)',text)[-1]
    game.send('#wizidentify\n',1); game.send('\t\n',1); game.settle()
    game.send('r'+tile); game.settle()
    game.lua('local o=u.inventory;while not o:isnull() do '
             'assert(o:totable().otyp_name~="syllable of spirit: Vaul","tile not consumed");o=o:next() end')
    game.save(); game.close(); game=Game(release,output,restore=True)
    for attempt in range(8):
        trigger()
        text=game.lua('nh.pline("VAUL_HP "..u.mh.." "..u.mhmax)')
        hp,maximum=map(int,re.findall(r'VAUL_HP (-?\d+) (\d+)',text)[-1])
        if hp<maximum:
            break
        # Native known traps have a 1/5 escape chance. Retry an avoided trap;
        # never count unchanged HP as evidence of damage protection.
        print('Known rust trap avoided; retrying',flush=True)
        game.send('h');game.settle()
    assert 0<hp<maximum, 'No protected rust damage observed; see terminal.txt'
    game.lua('assert(nh.int_to_pmname(u.umonnum)=="iron golem","restored Vaul did not protect iron form");'
             'assert(u.mh>0 and u.mh<u.mhmax,"rust did not damage protected form")')
    print('PASS pronounced Vaul survives save/reload and protects against actual rust-trap damage',flush=True)
finally:
    game.close()
