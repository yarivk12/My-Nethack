"""Wizard damage checks distinguish real weapon hits from bare attack dice."""
import re
import sys
from run_step8a_runtime import Game

release, output = sys.argv[1:3]
game = Game(release, output)
try:
    game.send('#levelchange\n'); game.wait('To what experience level')
    game.send('30\n'); game.settle()
    game.lua('nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()')
    game.send('#wizintrinsic\n'); game.wait('Which intrinsics?', more=False)
    menu = game.wait('sleep resistance', more=False)
    match = re.search(r'^\s*([A-Za-z])\s+[-+]\s+sleep resistance\b', menu, re.M)
    assert match, menu
    game.send('10000' + match[1] + '\n'); game.settle()

    def give(name):
        text = game.lua('local o=obj.new("' + name + '");u.giveobj(o);'
                        'nh.pline("FEY_ITEM "..o:totable().invlet)')
        return re.findall(r'FEY_ITEM (.)', text)[-1]

    whistle = give('tin whistle')

    def hp():
        text = game.lua('nh.pline("FEY_HP "..u.uhp)')
        return int(re.findall(r'FEY_HP (\d+)', text)[-1])

    for index, (name, cancelled, bare_max) in enumerate([
        ('Coure Eladrin', False, 16),
        ('Noviere Eladrin', False, 8),
        ('Noviere Eladrin', True, 8),
    ]):
        game.lua('nh.debug_flags({mongen=false})')
        heal = give('blessed potion of full healing')
        game.send('q' + heal); game.settle()
        game.lua('nh.debug_flags({mongen=true})')
        fixture = '''des.level_init({style="solidfill",fg=" "})
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
des.monster({id="NAME",coord={4,3},peaceful=false,asleep=true,cancelled=CANCELLED,
 keep_default_invent=false,inventory=function()
   des.object({id="rapier",spe=30,buc="uncursed"})
 end})
'''.replace('NAME', name).replace('CANCELLED', str(cancelled).lower())
        (game.path / 'fey-weapon.lua').write_text(fixture, encoding='utf8')
        game.send('#wizloaddes\n'); game.wait('Load which des lua file?')
        game.send('fey-weapon.lua\n', 2); game.settle()
        x, y = game.state()[2:]
        game.send('\x14'); game.wait('Where do you want to be teleported?'); game.settle()
        game.send(('l' if 4>x else 'h')*abs(4-x) + ('j' if 3>y else 'k')*abs(3-y) + '.')
        game.settle(); assert game.state()[2:] == (4,3)
        previous = hp()
        game.send('a' + whistle); game.settle()
        observed = 0
        for _ in range(3):
            current = hp(); observed = max(observed, previous-current)
            if observed > bare_max:
                break
            previous = current
            game.send('m.'); game.settle()
        assert observed > bare_max, (name, cancelled, observed, bare_max)
        print('PASS actual %s weapon damage %d exceeds bare-dice maximum %d; cancelled=%s'
              % (name, observed, bare_max, cancelled), flush=True)
        game.lua('nh.debug_flags({mongen=false})')
    game.save(); game.close(); game = Game(release, output, restore=True)
    print('PASS fey weapon fixture save/restore', flush=True)
finally:
    game.close()
