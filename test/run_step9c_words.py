"""Wizard study, active Words, terrain effects, cooldown and save/reload."""
import re
import sys
from run_step8a_runtime import Game as BaseGame, tty_transcript

class Game(BaseGame):
    def send(self, keys, delay=.3):
        if not hasattr(self,'observations'):
            self.observations=[]
        self.observations.append(self.text())
        super().send(keys,delay)
        self.observations.append(self.text())

release, output = sys.argv[1:3]
game = Game(release, output)
try:
    game.lua('nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()')
    game.send('#levelchange\n');game.wait('To what experience level')
    game.send('30\n');game.settle()
    (game.path / 'words.lua').write_text('''
des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip", "mazelevel")
des.map([[
----------------
|..............|
|..............|
|....Q}}.......|
|....e.........|
|..............|
----------------
]])
des.stair("down",1,1)
local x,y=nh.abscoord(0,0);nh.variable("word_origin",{x=x,y=y})
''', encoding='utf8')
    game.send('#wizloaddes\n')
    game.wait('Load which des lua file?')
    game.send('words.lua\n', 2)
    game.settle()
    game.send('#word\n', 1)
    game.settle()
    assert 'not learned any Words' in game.text(), game.text()

    for name in ['First Word', 'Dividing Word', 'Nurturing Word']:
        text = game.lua('local o=obj.new("' + name + '");u.giveobj(o);'
                        'nh.pline("SLAB_LETTER "..o:totable().invlet)')
        letter = re.findall(r'SLAB_LETTER (.)', text)[-1]
        game.send('i', 1)
        game.send('\x1b')
        game.settle()
        game.send('r' + letter, 1)
        game.wait('learn the ' + name + ' of Creation!', timeout=80)
        game.settle()
        game.lua('''local o=u.inventory
while not o:isnull() do assert(o:totable().otyp_name~="%s");o=o:next() end
''' % name)
        print('PASS actual study and consumption: ' + name, flush=True)

    # Stand west of the shallow/deep water sequence.
    a, b = game.state()[2:]
    text = game.lua('local p=nh.variable("word_origin");'
                    'nh.pline("WORD_TARGET "..(p.x+4).." "..(p.y+3))')
    tx, ty = map(int, re.findall(r'WORD_TARGET (\d+) (\d+)', text)[-1])
    game.send('\x14')
    game.wait('Where do you want to be teleported?')
    game.settle()
    game.send(('l' if tx > a else 'h') * abs(tx - a)
              + ('j' if ty > b else 'k') * abs(ty - b) + '.')
    game.settle()
    assert game.state()[2:] == (tx, ty)

    def word(letter, direction=None):
        raw_start = len(game.raw)
        observed_start=len(game.observations)
        game.send('#word\n', 1)
        game.wait('Speak which Word?', more=False)
        # PICK_ONE commits on the letter; Enter becomes a subsequent command.
        game.send(letter, 1)
        if direction:
            game.wait('In what direction?')
            game.send(direction)
        game.settle()
        return (tty_transcript(''.join(game.raw[raw_start:])) + game.text()
                + '\n'.join(game.observations[observed_start:]))

    def combatant(name,x,y):
        game.lua('nh.debug_flags({mongen=false});nh.debug_flags({mongen=true});'
                 'local p=nh.variable("word_origin");local ox,oy=nh.abscoord(0,0);'
                 'des.monster({id="%s",coord={p.x+%d-ox,p.y+%d-oy},'
                 'peaceful=false,paralyzed=126})' % (name,x,y))

    game.lua('local p=nh.variable("word_origin");local ox,oy=nh.abscoord(0,0);'
             'assert(not nh.getmap(p.x+5-ox,p.y+3-oy).lit)')
    combatant('kobold zombie',6,2)
    text=word('a')
    assert 'seared by the Light' in text, 'No First Word combat; see terminal.txt'
    print('PASS actual First Word combat against undead',flush=True)
    game.lua('local p=nh.variable("word_origin");local ox,oy=nh.abscoord(0,0);'
             'assert(nh.getmap(p.x+5-ox,p.y+3-oy).lit)')
    game.save()
    game.close()
    game = Game(release, output, restore=True)
    game.lua('nh.debug_flags({hunger=false,mongen=false})')
    text = word('a')
    assert 'cannot summon that Word again yet' in text, text
    print('PASS learned Word and active cooldown survive save/reload', flush=True)
    combatant('iron golem',8,3)
    text=word('b', 'l')
    assert 'thrown to the side' in text or 'bisected' in text, 'No Dividing combat; see terminal.txt'
    game.lua('''local p=nh.variable("word_origin");local ox,oy=nh.abscoord(0,0)
for x=5,7 do
 local m=nh.getmap(p.x+x-ox,p.y+3-oy)
 assert(m.mapchr==".")
 assert(m.has_trap==(x>5), "only deep water becomes a pit")
end
''')
    combatant('kobold zombie',6,2)
    word('c')
    game.lua('local p=nh.variable("word_origin");local ox,oy=nh.abscoord(0,0);'
             'assert(nh.getmap(p.x+6-ox,p.y+2-oy).mapchr=="T","Nurturing victim did not become a tree")')
    print('PASS actual Dividing combat and Nurturing undead death/tree creation',flush=True)
    game.lua('''local p=nh.variable("word_origin");local ox,oy=nh.abscoord(0,0)
assert(nh.getmap(p.x+4-ox,p.y+3-oy).mapchr=="G")
assert(nh.getmap(p.x+5-ox,p.y+4-oy).mapchr=="G")
''')
    game.save()
    game.close()
    game = Game(release, output, restore=True)
    game.lua('''local p=nh.variable("word_origin");local ox,oy=nh.abscoord(0,0)
assert(nh.getmap(p.x+5-ox,p.y+4-oy).mapchr=="G")
''')
    print('PASS First light, Dividing shallow/deep water, Nurturing room/soil and terrain restore', flush=True)
finally:
    game.close()
