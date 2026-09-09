"""Native Mithardir merchant menu, paid identification and save/reload.

An isolated sand-walker shop fixes geometry only. Merchant creation, saved
service choices, stock, billing and item identification use production code.
Portal guidance and the remaining services have separate coverage.
"""
from pathlib import Path
import re
import sys
from run_step8a_runtime import Game, tty_transcript

release, output = sys.argv[1:3]
shop = sys.argv[3] if len(sys.argv) > 3 else 'sand-walker shop'
assert shop in ('sea garden', 'fishery', 'sand-walker shop', 'spa')
game = Game(release, output)

def visit_services():
    game.send('p', 1)
    game.wait('Do you wish to try our other services?')
    game.send('y', 1)
    game.wait('Services available:', more=False)
    text = game.wait('Identify', more=False)
    return text

try:
    game.lua('nh.debug_flags({hunger=false,mongen=false});'
             'nh.debug_flags({mongen=true});u.clear_inventory()')
    fixture = '''des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip")
des.map({x=1,y=0,map=[=[
---------
|.......|
|.......|
+.......|
|.......|
|.......|
---------
]=]})
des.door("closed",0,3)
des.region({region={1,1,7,5},lit=1,type="sand-walker shop",filled=1})
'''.replace('sand-walker shop', shop)
    (game.path / 'merchant.lua').write_text(fixture, encoding='utf8')
    game.send('#wizloaddes\n'); game.wait('Load which des lua file?')
    game.send('merchant.lua\n', 2); game.settle()
    x, y = game.state()[2:]
    game.send('\x14'); game.wait('Where do you want to be teleported?')
    game.settle()
    game.send(('l' if 3>x else 'h')*abs(3-x) + ('j' if 3>y else 'k')*abs(3-y) + '.')
    game.settle()
    assert game.state()[2:] == (3, 3)
    game.lua('u.giveobj(obj.new("10000 gold pieces"));'
             'u.giveobj(obj.new("uncursed syllable of strength: Aesh"))')
    game.send('i', 1); game.send('\x1b'); game.settle()
    text = game.lua('''local o=u.inventory
while not o:isnull() do local t=o:totable()
 if t.otyp_name=="syllable of strength: Aesh" then
  nh.pline("TILE_LETTER "..t.invlet)
 end
 o=o:next()
end''')
    tile = re.findall(r'TILE_LETTER (.)', text)[-1]
    game.send('r' + tile, 1); game.settle()
    assert "don't know how to pronounce" in game.text(), game.text()
    before_gold = int(re.findall(r'\$:(\d+)', game.text())[-1])
    menu = visit_services()
    offerings = tuple(name for name in ['Identify', 'Uncurse', 'Weapon-works'] if name in menu)
    assert 'Identify' in offerings
    game.send('i', 1)
    game.wait('have identified')
    game.send(tile, 1)
    game.settle()
    if 'Basic service or premier?' in game.text():
        game.send('b', 1)
    game.wait('Interested?')
    game.send('y', 1)
    game.wait('have identified')
    game.send('\x1b'); game.settle()
    after_gold = int(re.findall(r'\$:(\d+)', game.text())[-1])
    assert after_gold < before_gold
    # A tile sold by this shop is never guesswork. Successful pronunciation
    # is a behavior check of type identification, regardless of basic/premier.
    game.send('r' + tile, 1); game.settle()
    assert "don't know how to pronounce" not in game.text(), game.text()
    game.lua('''local o=u.inventory
while not o:isnull() do
 assert(o:totable().otyp_name~="syllable of strength: Aesh","tile not consumed")
 o=o:next()
end''')
    print('PASS native %s services, paid tile identification and pronunciation' % shop, flush=True)
    game.save(); game.close(); game = Game(release, output, restore=True)
    menu = visit_services()
    assert tuple(name for name in ['Identify', 'Uncurse', 'Weapon-works'] if name in menu) == offerings
    game.send('\x1b'); game.settle()
    print('PASS merchant, service availability and paid transaction save/reload', flush=True)
finally:
    game.close()
