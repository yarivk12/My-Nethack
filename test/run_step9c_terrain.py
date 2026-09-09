"""Actual wizard shallow-water freeze/thaw/evaporation and saved terrain.

Uses an isolated arena, not a production branch-placement override.
"""
import re
import sys
from run_step8a_runtime import Game

release, output = sys.argv[1:3]
game = Game(release, output)
try:
    game.lua('nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()')
    (game.path / 'shallow.lua').write_text('''
des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip", "mazelevel")
des.map([[
---------------
|.............|
|...Q.........|
|.............|
|...seG.......|
|.............|
---------------
]])
des.region(selection.area(1,1,13,5),"lit")
des.stair("down",1,1)
local x,y=nh.abscoord(0,0);nh.variable("shallow_origin",{x=x,y=y})
''', encoding='utf8')
    game.send('#wizloaddes\n')
    game.wait('Load which des lua file?')
    game.send('shallow.lua\n', 2)
    game.settle()
    text = game.lua('''
for _,name in ipairs({"fire","cold"}) do
 local o=obj.new("wand of "..name.." (0:20)");u.giveobj(o)
 nh.pline("TOOL "..name.." "..o:totable().invlet)
end
local p=nh.variable("shallow_origin")
nh.pline(string.format("TARGET %d %d %d %d",u.ux,u.uy,p.x+3,p.y+2))
''')
    letters = dict(re.findall(r'TOOL (fire|cold) (.)', text))
    a, b, tx, ty = map(int, re.findall(r'TARGET (\d+) (\d+) (\d+) (\d+)', text)[-1])
    if (a, b) != (tx, ty):
        game.send('\x14')
        game.wait('Where do you want to be teleported?')
        game.settle()
        game.send(('l' if tx > a else 'h') * abs(tx - a)
                  + ('j' if ty > b else 'k') * abs(ty - b) + '.')
        game.settle()
    assert game.state()[2:] == (tx, ty)

    def check(x, y, terrain):
        game.lua('local p=nh.variable("shallow_origin");local ox,oy=nh.abscoord(0,0);'
                 'assert(nh.getmap(p.x+%d-ox,p.y+%d-oy).mapchr=="%s",'
                 '"wrong Mithardir terrain")' % (x, y, terrain))

    def zap(name):
        game.send('z' + letters[name])
        game.wait('In what direction?')
        game.send('l')
        game.settle()

    check(4, 2, 'Q')
    for x, symbol in [(4, 's'), (5, 'e'), (6, 'G')]:
        check(x, 4, symbol)
    zap('cold')
    check(4, 2, 'I')
    game.save()
    game.close()
    game = Game(release, output, restore=True)
    check(4, 2, 'I')
    zap('fire')
    check(4, 2, 'Q')
    zap('fire')
    check(4, 2, '.')
    print('PASS actual shallow-water freezing, saved thaw identity, and evaporation', flush=True)
    game.save()
    game.close()
    game = Game(release, output, restore=True)
    check(4, 2, '.')
    for x, symbol in [(4, 's'), (5, 'e'), (6, 'G')]:
        check(x, 4, symbol)
    print('PASS saved white dust, soil, grass and evaporated shallow water', flush=True)
finally:
    game.close()
