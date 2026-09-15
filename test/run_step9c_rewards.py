"""Wizard syllable identification, timed/permanent AC, masks and save/reload.

This focused check does not claim active Word or full Mithardir validation.
"""
import re
import sys
import time
from run_step8a_runtime import Game

release, output = sys.argv[1:3]
game = Game(release, output)
try:
    game.lua('nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()')

    def give(name):
        text = game.lua('local o=obj.new("' + name + '");u.giveobj(o);'
                        'nh.pline("ITEM_LETTER "..o:totable().invlet)')
        game.send('i', 1)
        game.send('\x1b', .2)
        game.settle()
        return re.findall(r'ITEM_LETTER (.)', text)[-1]

    def ac():
        game.settle()
        return int(re.findall(r'AC:\s*(-?\d+)', game.text())[-1])

    def identify():
        game.send('#wizidentify\n', 1)
        game.send('\t\n', 1)
        game.settle()

    def world_turn():
        text = game.lua('nh.pline("WORLD_TURN "..u.moves)')
        return int(re.findall(r'WORLD_TURN (\d+)', text)[-1])

    def wait_world(turns):
        start = world_turn()
        for batch in range(30):
            for _ in range(5):
                game.send('.', .2)
                game.settle()
            current = world_turn()
            print('WORLD elapsed=%d actions=%d AC=%d' %
                  (current - start, (batch + 1) * 5, ac()), flush=True)
            if current - start >= turns:
                return
        raise AssertionError('World turns failed to advance\n' + game.text())

    initial = ac()
    tile = give('uncursed syllable of grace: Uur')
    game.send('r' + tile, 1)
    game.settle()
    assert "don't know how to pronounce" in game.text(), game.text()
    assert ac() == initial
    identify()
    game.send('r' + tile, 1)
    game.settle()
    assert ac() == initial - 11, game.text()
    print('PASS pronunciation requires identification; uncursed Uur gives timed and permanent AC', flush=True)
    game.save()
    game.close()
    game = Game(release, output, restore=True)
    assert ac() == initial - 11, game.text()
    wait_world(12)
    assert ac() == initial - 1, game.text()
    print('PASS timed syllable and permanent count survive restore; timer expires independently', flush=True)
    cursed = give('cursed syllable of grace: Uur')
    game.send('r' + cursed, 1)
    game.settle()
    assert ac() == initial - 11, game.text()
    wait_world(42)
    assert ac() == initial - 1, game.text()
    print('PASS cursed Uur lasts forty turns without adding a permanent count', flush=True)
    mask = give('uncursed living mask')
    game.send('P' + mask, 1)
    game.settle()
    game.lua('''local o=u.inventory;local found=false
while not o:isnull() do local t=o:totable()
 if t.otyp_name=="living mask" then assert(t.owornmask~=0);found=true end
 o=o:next()
end
assert(found)
''')
    game.send('#attributes\n', 1)
    game.wait("Wizard's attributes:", more=False)
    deadline = time.monotonic() + 40
    while time.monotonic() < deadline:
        text = game.text()
        if re.search(r'\((?:end|\d+ of \d+)\)', text):
            break
        time.sleep(.1)
    else:
        raise AssertionError('Incomplete attributes menu\n' + text)
    pages = [text]
    page = re.search(r'\((\d+) of (\d+)\)', text)
    if page:
        for index in range(int(page[1]) + 1, int(page[2]) + 1):
            game.send(' ', 1)
            pages.append(game.wait('(%d of %s)' % (index, page[2]), more=False))
    text = '\n'.join(pages)
    (game.path / 'living-mask-attributes.txt').write_text(text, encoding='utf8')
    assert 'survive without air' in text and 'You are blind' not in text, text
    game.send('\x1b')
    game.settle()
    game.save()
    game.close()
    game = Game(release, output, restore=True)
    assert ac() == initial - 1
    game.lua('''local o=u.inventory;local found=false
while not o:isnull() do local t=o:totable()
 if t.otyp_name=="living mask" then assert(t.owornmask~=0);found=true end
 o=o:next()
end
assert(found)
''')
    print('PASS living mask wears without blindness and remains worn after restore', flush=True)
finally:
    game.close()
