"""Actual wizard-game save, restore, leave/revisit and recurrence check."""
from pathlib import Path
import os
import re
import subprocess
import sys
from run_step8a_runtime import Game, tty_transcript

release, output = sys.argv[1:3]
os.environ["CUSTOMROOM"] = "4"
game = Game(release, output)


def identity(g):
    start = len(g.raw)
    g.send("#WIZCUSTOMROOMS\n", 1)
    g.settle()
    text = tty_transcript("".join(g.raw[start:])) + "\n" + g.text()
    return list(dict.fromkeys(re.findall(r"Custom ID 4, room \d+, bounds \d+,\d+-\d+,\d+, type \d+/\d+",text)))


def level(g, dl):
    g.send("\x16"); g.wait("To what level"); g.send(str(dl)+"\n",1); g.settle()


try:
    game.lua("nh.debug_flags({hunger=false,mongen=false})")
    initial = None
    for depth in range(40, 46):
        level(game, depth)
        ids = identity(game)
        if ids:
            initial = (depth, ids)
            break
    assert initial
    before = game.state()
    game.save()
    game = Game(release, output, restore=True)
    assert game.state() == before
    assert identity(game) == initial[1]
    repeated = []
    for other in range(initial[0] + 1, initial[0] + 9):
        level(game, other)
        repeated = identity(game)
        if repeated:
            break
    assert repeated, "no recurrence on subsequent eligible levels"
    level(game, initial[0])
    assert identity(game) == initial[1]
    game.save()
    game = Game(release, output, restore=True)
    assert identity(game) == initial[1]
    print("PASS full game save/restore and leave/revisit: identity stable; recurrence", bool(repeated), flush=True)
finally:
    game.close()

# The final restored game above was terminated with its native checkpoint
# intact. Recover only this newly-created fixture, then restore the result.
recovery = subprocess.run([str(Path(release).resolve() / "recover.exe"),
                           "-d", str(Path(output).resolve()), "wizard"],
                          capture_output=True, text=True)
(Path(output) / "recovery.log").write_text(recovery.stdout + recovery.stderr)
assert recovery.returncode == 0, recovery.stdout + recovery.stderr
game = Game(release, output, restore=True)
try:
    assert identity(game) == initial[1]
    print("PASS actual recover.exe checkpoint output restored with custom identity", flush=True)
finally:
    game.close()
