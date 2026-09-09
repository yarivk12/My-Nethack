"""Wizard probing verifies tiny Coure armor is equipped and survives restore."""
import re
import sys
from run_step8a_runtime import Game, tty_transcript

release, output = sys.argv[1:3]
game = Game(release, output)
try:
    game.lua('nh.debug_flags({hunger=false,mongen=false});'
             'nh.debug_flags({mongen=true});u.clear_inventory()')
    (game.path / 'coure.lua').write_text('''des.level_init({style="solidfill",fg=" "})
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
des.monster({id="Coure Eladrin",coord={4,3},peaceful=true,paralyzed=126})
''', encoding='utf8')
    game.send('#wizloaddes\n'); game.wait('Load which des lua file?')
    game.send('coure.lua\n', 2); game.settle()
    x, y = game.state()[2:]
    game.send('\x14'); game.wait('Where do you want to be teleported?'); game.settle()
    game.send(('l' if 4>x else 'h')*abs(4-x) + ('j' if 3>y else 'k')*abs(3-y) + '.')
    game.settle(); assert game.state()[2:] == (4,3)
    text = game.lua('local o=obj.new("wand of probing (0:10)");u.giveobj(o);'
                    'nh.pline("PROBE_LETTER "..o:totable().invlet)')
    letter = re.findall(r'PROBE_LETTER (.)', text)[-1]

    def probe():
        start = len(game.raw)
        game.send('z' + letter); game.wait('In what direction?'); game.send('l', 1)
        game.wait('being worn')
        game.settle()
        text = tty_transcript(''.join(game.raw[start:])) + game.text()
        (game.path / ('probe-%d.txt' % game.seq)).write_text(text, encoding='utf8')
        assert text.count('(being worn)') >= 3, text
        assert 'tiny' in text and 'jacket' in text, text
        game.send('\x1b'); game.settle()

    probe()
    game.save(); game.close(); game = Game(release, output, restore=True)
    probe()
    print('PASS native tiny Coure armor equipped, probed and saved/restored', flush=True)
finally:
    game.close()
