"""Wizard-load the real statue map and verify native objects/display/restore.

Only the not-yet-connected portal and unrelated ooze are suppressed in the
isolated fixture. Production statue generation and object parsing run intact.
"""
from pathlib import Path
import re
import sys
from run_step8a_runtime import Game, tty_transcript

repo = Path(__file__).resolve().parents[1]
release, output = sys.argv[1:3]
game = Game(release, output)
try:
    game.lua('nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()')
    source = (repo / 'dat/cat1.lua').read_text(encoding='utf8')
    source, n = re.subn(r'^des\.levregion\(.*\)\s*$', '', source, flags=re.M)
    assert n == 1
    source, n = re.subn(r'^des\.monster\(.*\)\s*$', '', source, flags=re.M)
    assert n == 1
    # Exercise every body resolution once, then repeat the first identity.
    source = '''local actual_rn2=nh.rn2
local face=0
nh.rn2=function(n)
 if n==30 then local r=face%30;face=face+1;return r end
 return actual_rn2(n)
end
''' + source + '\nnh.rn2=actual_rn2\n'
    (game.path / 'statues.lua').write_text(source, encoding='utf8')
    game.send('#wizloaddes\n')
    game.wait('Load which des lua file?')
    game.send('statues.lua\n', 2)
    game.settle()
    game.lua('''local count=0;local chosen=nil;local o=obj.next()
while not o:isnull() do
 local t=o:totable()
 if t.otyp_name=="statue" then
  count=count+1;assert(t.blessed==1 and t.historic==1)
  assert(t.corpsenm_name and t.corpsenm_name~="")
  if t.corpsenm_name=="elven monarch" and t.has_oname==0 then chosen=o end
 end
 o=o:next()
end
assert(count==31,"wrong native statue count");assert(chosen)
u.giveobj(chosen)
''')

    def check_display():
        start = len(game.raw)
        game.send('i', 1)
        game.settle()
        text = tty_transcript(''.join(game.raw[start:])) + game.text()
        assert 'faceless statue' in text, text
        game.send('\x1b')
        game.settle()

    check_display()
    game.save()
    game.close()
    game = Game(release, output, restore=True)
    check_display()
    game.lua('''local t=u.inventory:totable()
assert(t.otyp_name=="statue" and t.blessed==1 and t.historic==1)
local count=0;local o=obj.next()
while not o:isnull() do
 if o:totable().otyp_name=="statue" then count=count+1 end
 o=o:next()
end
assert(count==30)
''')
    print('PASS all 30 native body/name resolutions, 31 historic statues, faceless display and save/reload', flush=True)
finally:
    game.close()
