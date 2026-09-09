"""Wizard observation of an elder's actual support magic, then save/restore."""
import re
import sys
from run_step8a_runtime import Game


class ObservedGame(Game):
    """Capture complete terminal state before advancing each More prompt."""
    def __init__(self, *args, **kwargs):
        self.observed = []
        super().__init__(*args, **kwargs)

    def send(self, keys, delay=.3):
        if hasattr(self, 'screen'):
            self.observed.append(self.text())
        super().send(keys, delay)
        if hasattr(self, 'screen'):
            self.observed.append(self.text())

release, output = sys.argv[1:3]
game = ObservedGame(release, output)
try:
    game.send('#levelchange\n'); game.wait('To what experience level')
    game.send('30\n'); game.settle()
    game.lua('nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()')

    def give(name):
        text = game.lua('local o=obj.new("' + name + '");u.giveobj(o);'
                        'nh.pline("ELDER_ITEM "..o:totable().invlet)')
        return re.findall(r'ELDER_ITEM (.)', text)[-1]

    # These wizard protections let the test keep observing after status spells.
    armor = give('blessed +3 gray dragon scale mail')
    game.send('W' + armor); game.settle()
    eyes = give('The Eyes of the Overworld')
    game.send('P' + eyes); game.settle()
    see = give('blessed potion of see invisible')
    game.send('q' + see); game.settle()
    game.send('#polyself\n'); game.wait('Become what kind of monster?')
    game.send('vampire\n'); game.settle()
    game.lua('assert(nh.int_to_pmname(u.umonnum)=="vampire")')
    strike = give('wand of magic missile (0:30)')
    game.lua('nh.debug_flags({mongen=true})')
    (game.path / 'elder.lua').write_text('''
des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip")
des.map({x=1,y=0,map=[=[
---------------
|.............|
|.............|
|.............|
|.............|
|.............|
---------------
]=]})
des.region(selection.area(1,1,13,5),"lit")
des.terrain({coord={7,3},typ="F"})
for x=7,9 do
  des.terrain({coord={x,2},typ="|"})
  des.terrain({coord={x,4},typ="|"})
end
des.terrain({coord={9,3},typ="|"})
des.monster({id="Alabaster elf-elder",coord={8,3},peaceful=false,asleep=true,
             keep_default_invent=false})
''', encoding='utf8')
    game.send('#wizloaddes\n'); game.wait('Load which des lua file?')
    game.send('elder.lua\n', 2); game.settle()
    x, y = game.state()[2:]
    game.send('\x14'); game.wait('Where do you want to be teleported?'); game.settle()
    # Stand immediately outside the bars so summoned monsters cannot intercept
    # the beam in an intervening open square. The elder is still at range two.
    game.send(('l' if 7>x else 'h')*abs(7-x) + ('j' if 3>y else 'k')*abs(3-y) + '.')
    game.settle(); assert game.state()[2:] == (7,3)
    start = len(game.observed)
    game.send('z' + strike); game.wait('In what direction?'); game.send('l'); game.settle()
    print('Elder fixture ready; observing real spell turns', flush=True)
    for turn in range(96):
        text = '\n'.join(game.observed[start:]) + game.text()
        if 'looks better' in text and ('sleep' in text.lower() or 'drowsy' in text.lower()):
            break
        game.send('m.'); game.settle()
        if turn % 16 == 15:
            print('Observed %d elder spell turns' % (turn + 1), flush=True)
    text = '\n'.join(game.observed[start:]) + game.text()
    (game.path / 'elder-spells.txt').write_text(text, encoding='utf8')
    assert 'looks better' in text, 'No elder healing; see elder-spells.txt'
    assert 'sleep' in text.lower() or 'drowsy' in text.lower(), \
        'No sleep beam observed; see elder-spells.txt'
    game.save(); game.close(); game = ObservedGame(release, output, restore=True)
    game.lua('assert(nh.int_to_pmname(u.umonnum)=="vampire")')
    print('PASS actual elder group healing, sleep beam and save/restore', flush=True)
finally:
    game.close()
