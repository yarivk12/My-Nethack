"""Traverse the packaged Sheol branch in wizard mode, preserving save state.

All generated games and logs are written to a new external output directory.
Usage: python run_step9a_runtime.py RELEASE_DIRECTORY NEW_OUTPUT_DIRECTORY
"""
from pathlib import Path
import sys
from run_step8a_runtime import Game


def main():
    release, output = sys.argv[1:3]
    game = Game(release, output)
    def secure_floor():
        # This runner verifies connectors and persistent geometry. Combat,
        # unique births/deaths and rewards have separate fixtures. Remove
        # combatants after inspecting each generated floor, then reenable
        # generation before traversing to the next one.
        game.lua('nh.debug_flags({hunger=false,mongen=false}); nh.debug_flags({mongen=true})')
    try:
        game.send('#levelchange\n')
        game.wait('To what experience level')
        game.send('30\n')
        game.settle()
        game.lua('nh.debug_flags({hunger=false})')
        parent = game.branch_depth('Sheol')
        assert 30 <= parent <= 199
        sheol = game.dungeon_number('Sheol')
        game.send('\x16')
        game.wait('To what level')
        game.send(str(parent) + '\n', 1)
        game.settle()
        assert game.state()[:2] == (0, parent)
        assert game.stair('down', True, sheol)[:2] == (sheol, 1)
        game.lua(Path(__file__).with_name('test_step9a.lua').read_text())
        secure_floor()
        game.save()
        game.close()
        game = Game(release, output, restore=True)
        game.lua('nh.debug_flags({hunger=false})')
        assert game.state()[:2] == (sheol, 1)
        print('PASS filler save/restore', flush=True)
        count = 1
        while True:
            text = game.lua('''local n=0
for _,s in ipairs(nh.stairways()) do
 if not s.up and s.dnum==u.dnum then n=n+1 end
end
nh.pline(string.format("DOWN_COUNT %d",n))''')
            if 'DOWN_COUNT 0' in text:
                break
            assert 'DOWN_COUNT 1' in text, text
            count += 1
            assert count <= 8
            assert game.stair('down')[:2] == (sheol, count)
            game.lua(Path(__file__).with_name('test_step9a.lua').read_text())
            secure_floor()
        assert count in (6, 7, 8)
        game.save()
        game.close()
        game = Game(release, output, restore=True)
        game.lua('nh.debug_flags({hunger=false})')
        assert game.state()[:2] == (sheol, count)
        print('PASS terminal save/restore', flush=True)
        for level in range(count-1, 0, -1):
            assert game.stair('up')[:2] == (sheol, level)
        assert game.stair('up', True, 0)[:2] == (0, parent)
        print('PASS Sheol', count, 'levels; complete round trip to DoD%d' % parent, flush=True)
    finally:
        game.close()


if __name__ == '__main__':
    main()
