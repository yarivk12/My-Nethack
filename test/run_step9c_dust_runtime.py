"""Observe natural Wastes dust regions through wizard #timeout and restore."""
import re
import sys
from run_step8a_runtime import Game

release, output = sys.argv[1:3]
game = Game(release, output)
pattern = r'(\d+)\s+dust storm \((\d+)\)\s+@\[(-?\d+),(-?\d+)\.\.(-?\d+),(-?\d+)\]'

def regions():
    game.send('#timeout\n', 1)
    text = game.text()
    for _ in range(10):
        if '(end)' in game.text():
            break
        if '--More--' in game.text() or re.search(r'\(\d+ of \d+\)',game.text()):
            game.send(' ', .4)
            text += '\n' + game.text()
        else:
            break
    game.send('\x1b');game.settle()
    return sorted(set(tuple(map(int,m)) for m in re.findall(pattern,text)))

try:
    game.lua('nh.debug_flags({hunger=false,mongen=false})')
    game.send('#levelchange\n');game.wait('To what experience level')
    game.send('30\n');game.settle()
    game.send('\x16');game.wait('To what level');game.send('?\n',1)
    # Named destinations only work within the current branch. The wizard
    # dungeon menu is the native cross-branch path.
    destination=None
    for _ in range(12):
        text=game.text()
        match=re.search(r'^\s*(.)\s+[-+]\s+mith1\b',text,re.M)
        if match:
            destination=match[1];break
        game.send('>',.6)
    assert destination, 'Wastes missing from wizard destination menu; see terminal.txt'
    game.send(destination,1);game.settle()
    assert game.state()[:2] == (game.dungeon_number('Mithardir'),2)
    game.lua('nh.debug_flags({mongen=false})')
    # A breathing-immune form lets the environment evolve without a combat
    # or survival fixture changing the storm population.
    game.send('#polyself\n');game.wait('Become what kind of monster?')
    game.send('iron golem\n');game.settle()
    before=[]
    for attempt in range(40):
        before=regions()
        if any(r[0]>1 for r in before):
            break
        for _ in range(5):
            game.send('.',.15);game.settle()
    assert any(r[0]>1 for r in before), 'No live natural Wastes storm observed; see terminal.txt'
    print('PASS naturally generated Wastes dust regions:',before,flush=True)
    game.save();game.close();game=Game(release,output,restore=True)
    restored=regions()
    # #timeout displays ttl+1; native rest_regions discards ttl==0 without
    # firing expiry callbacks. Preserve that existing save/restore rule.
    expected=[r for r in before if r[0]>1]
    assert restored==expected, (before,restored)
    print('PASS exact dust strength, bounds and remaining lifetime survive restore',flush=True)
    for _ in range(12):
        game.send('.',.15);game.settle()
    after=regions()
    assert after!=before, 'Dust did not age after restore'
    assert not (game.path/'paniclog').exists(), 'Dust runtime panic; see paniclog'
    print('PASS restored natural storms continue expiring/drifting; no runtime panic',flush=True)
finally:
    game.close()
