"""Actual wizard melee verifies the imported offhand survives save/restore."""
import sys
from run_step8a_runtime import Game, tty_transcript

release, output = sys.argv[1:3]
game = Game(release, output)
try:
    game.send('#levelchange\n'); game.wait('To what experience level')
    game.send('30\n'); game.settle()
    game.lua('nh.debug_flags({hunger=false,mongen=false});'
             'nh.debug_flags({mongen=true});u.clear_inventory()')
    (game.path / 'offhand.lua').write_text('''
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
des.monster({id="Bralani Eladrin",coord={4,3},peaceful=false,asleep=true,
  keep_default_invent=false,inventory=function()
    des.object({id="long sword",spe=0,buc="uncursed"})
    des.object({id="knife",spe=0,buc="uncursed"})
  end})
''', encoding='utf8')
    game.send('#wizloaddes\n'); game.wait('Load which des lua file?')
    game.send('offhand.lua\n', 2); game.settle()
    x, y = game.state()[2:]
    game.send('\x14'); game.wait('Where do you want to be teleported?'); game.settle()
    game.send(('l' if 4>x else 'h')*abs(4-x) + ('j' if 3>y else 'k')*abs(3-y) + '.')
    game.settle(); assert game.state()[2:] == (4,3)

    def combat(label, wake=False):
        start = len(game.raw)
        game.send('l' if wake else 'm.'); game.settle()
        for _ in range(4):
            text = tty_transcript(''.join(game.raw[start:])) + game.text()
            if 'long sword' in text and 'knife' in text:
                break
            game.send('m.'); game.settle()
        text = tty_transcript(''.join(game.raw[start:])) + game.text()
        (game.path / (label + '.txt')).write_text(text, encoding='utf8')
        assert 'long sword' in text and 'knife' in text, text
        assert 'swings' in text or 'thrusts' in text, text
        print('PASS actual Bralani mainhand/offhand melee: ' + label, flush=True)

    combat('before-save', wake=True)
    game.save(); game.close(); game = Game(release, output, restore=True)
    combat('after-restore')
finally:
    game.close()
