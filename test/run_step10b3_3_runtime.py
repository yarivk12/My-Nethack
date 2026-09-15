"""Packaged runtime coverage for the Step 10B3-3 artifacts."""
import json
import re
import sys
import time
from pathlib import Path

import run_step8a_runtime as runtime_harness
from run_step8a_runtime import Game, tty_transcript


# Use the packaged curses port for reliable extended-command/menu input under
# WinPTY, matching the established B3-2 artifact fixture.
_spawn = runtime_harness.PtyProcess.spawn


def _spawn_curses(*args, **kwargs):
    env = dict(kwargs.get("env", {}))
    env["NETHACKOPTIONS"] = env.get("NETHACKOPTIONS", "").replace(
        "windowtype:tty", "windowtype:curses")
    kwargs["env"] = env
    return _spawn(*args, **kwargs)


runtime_harness.PtyProcess.spawn = _spawn_curses


def _settle_curses(game):
    for _ in range(40):
        if "--More--" in game.text():
            game.send(" ", 1)
        else:
            time.sleep(1)
            return
    raise AssertionError("Unending messages\n" + game.text())


runtime_harness.Game.settle = _settle_curses
_send = runtime_harness.Game.send


def _send_curses(game, keys, delay=.3):
    return _send(game, keys.replace("\n", "\r"), delay)


runtime_harness.Game.send = _send_curses


def _save_curses(game):
    game.settle()
    savefile = game.path / "wizard.NetHack-saved-game"
    before = ((savefile.stat().st_mtime_ns, savefile.stat().st_size)
              if savefile.exists() else (0, 0))
    game.send("S")
    game.wait("Really save?")
    game.send("y", 1)
    deadline = time.monotonic() + 20
    while time.monotonic() < deadline:
        if savefile.exists():
            current = (savefile.stat().st_mtime_ns, savefile.stat().st_size)
            if current != before:
                break
        time.sleep(.2)
    else:
        raise AssertionError("save file was not updated\n" + game.text())
    if game.p.isalive():
        game.p.terminate(force=True)
    game.thread.join(timeout=3)
    (game.path / "before-restore-terminal.txt").write_text(
        "".join(game.raw), encoding="utf8")


runtime_harness.Game.save = _save_curses


release, output_arg = sys.argv[1:3]
output = Path(output_arg).resolve()
LEVEL = 16
NATIVE_REGEN_PERIOD = (30 + 8 - LEVEL) * 3 // 6


def give(game, specification, expected_name=None):
    encoded = json.dumps(specification)
    check = "" if expected_name is None else (
        "assert(t.oname==%s,'wrong artifact name '..tostring(t.oname));"
        % json.dumps(expected_name))
    text = game.lua(
        "local o=obj.new(%s);local t=o:totable();%s"
        "u.giveobj(o);nh.pline('B33_ITEM '..o:totable().invlet)"
        % (encoded, check))
    found = re.findall(r"B33_ITEM (.)", text)
    assert found, text
    return found[-1]


def values(game):
    text = game.lua(
        'nh.pline(string.format("B33_VALUES %d %d %d",u.uen,u.uenmax,u.moves))')
    found = re.findall(r"B33_VALUES (\d+) (\d+) (\d+)", text)
    assert found, text
    return tuple(map(int, found[-1]))


def align_moves(game, modulus):
    current = values(game)[2]
    turns = (modulus - current) % NATIVE_REGEN_PERIOD
    if turns:
        game.send("." * turns, max(1, turns * .15))
        game.settle()
    assert values(game)[2] % NATIVE_REGEN_PERIOD == modulus


def attributes(game, label):
    game.send("#attributes\n", 1)
    game.wait("Wizard's attributes:", more=False)
    time.sleep(1)
    text = game.text()
    (game.path / (label + ".txt")).write_text(text, encoding="utf8")
    game.send("\x1b")
    game.settle()
    return text.lower()


def read_passage(game, letter, accelerator):
    align_moves(game, 0)
    before = values(game)
    raw_start = len(game.raw)
    game.send("r")
    game.wait("What do you want to read?")
    game.send(letter)
    game.wait("Which passage will you read?", more=False)
    game.send(accelerator, 1)
    # Detection can leave its native map display waiting for dismissal.
    game.send("\x1b", 1)
    game.settle()
    transcript = tty_transcript("".join(game.raw[raw_start:])) + "\n" + game.text()
    after = values(game)
    assert after[2] == before[2] + 1, (accelerator, before, after, transcript)
    return before, after, transcript


def inventory_names(game, label):
    # Permanently identify every item first; the instance-material prefix is
    # intentionally hidden until dknown/known state permits it.
    game.send("#wizidentify\n", 1)
    game.wait("Debug Identify", more=False)
    game.send("\t\n", 1)
    game.settle()
    raw_start = len(game.raw)
    game.send("i", 1)
    time.sleep(1)
    text = tty_transcript("".join(game.raw[raw_start:])) + "\n" + game.text()
    (game.path / (label + ".txt")).write_text(text, encoding="utf8")
    game.send("\x1b", 1)
    game.settle()
    return text


def find_inventory(game):
    text = game.lua('''local o=u.inventory
while not o:isnull() do
 local t=o:totable()
 if t.has_oname~=0 then
  nh.pline(string.format("B33_INV %s|%s|%d|",t.invlet,t.oname,t.age))
 else
  nh.pline(string.format("B33_BASE %s|%s|",t.invlet,t.otyp_name))
 end
 o=o:next()
end''')
    artifacts = {
        name: (letter, int(age))
        for letter, name, age in re.findall(r"B33_INV (.)\|(.+?)\|(\d+)\|", text)
    }
    bases = {
        name: letter for letter, name in re.findall(r"B33_BASE (.)\|(.+?)\|", text)
    }
    return artifacts, bases


def main():
    game = Game(release, output)
    try:
        game.lua("nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()")
        game.send("#levelchange\n")
        game.wait("To what experience level")
        # Level 16 supplies ample Pw but precedes the Wizard's native
        # teleport-control intrinsic, so carrier gain/removal is observable.
        game.send(str(LEVEL) + "\n")
        game.settle()

        necro = give(game, "uncursed The Necronomicon", "The Necronomicon")
        ordinary_secrets = give(game, "blessed spellbook of secrets")
        ordinary_key = give(game, "uncursed universal key")

        # Menu cancellation is free and does not mutate resource state.
        before = values(game)
        raw_start = len(game.raw)
        game.send("r")
        game.wait("What do you want to read?")
        game.send(necro)
        game.wait("Which passage will you read?", more=False)
        game.send("\x1b", 1)
        game.settle()
        after = values(game)
        assert after == before, (before, after, game.text())
        cancelled = tty_transcript("".join(game.raw[raw_start:]))
        assert "Which passage will you read?" in cancelled

        # Every retained passage executes through the packaged read/menu path.
        # The debug suppression used to remove startup monsters also blocks
        # explicit makemon(), so re-enable it before testing summon passages.
        game.lua("nh.debug_flags({mongen=true})")
        for accelerator, cost, phrase in (
                ("a", 20, "byakhee answers"),
                ("b", 10, "night-gaunt answers"),
                ("c", 5, ""),
                ("d", 0, "Nothing happens")):
            if accelerator in ("a", "b"):
                game.lua("nh.debug_flags({mongen=true})")
            before, after, transcript = read_passage(game, necro, accelerator)
            assert before[0] - after[0] == cost, (
                accelerator, before, after, transcript)
            if phrase:
                assert phrase.lower() in transcript.lower(), transcript
            if accelerator in ("a", "b"):
                # Suppress the imported species' normal ambient turns after
                # proving the artifact summon/taming path; the next passage
                # is not a monster-AI integration test.
                game.lua("nh.debug_flags({mongen=false})")

        # The ordinary base retains B3-1's harmless one-line completion and
        # never enters the artifact menu.
        raw_start = len(game.raw)
        game.send("r")
        game.wait("What do you want to read?")
        game.send(ordinary_secrets)
        text = game.wait("ragged pages hint at secrets beyond mortal spellcraft")
        game.settle()
        ordinary_output = (tty_transcript("".join(game.raw[raw_start:]))
                           + "\n" + text)
        assert "Which passage will you read?" not in ordinary_output

        absent = attributes(game, "silver-key-absent")
        assert "teleport control" not in absent
        assert "polymorph control" not in absent

        silver = give(game, "uncursed The Silver Key", "The Silver Key")
        present = attributes(game, "silver-key-carried")
        assert "teleport control" in present
        assert "polymorph control" in present

        names = inventory_names(game, "identified-artifacts")
        # Proper artifact names intentionally bypass xname()'s instance
        # material adjective; the focused production-path test verifies the
        # SILVER assignment.  Here the ordinary and artifact identities must
        # remain distinct through real creation and inventory handling.
        assert "the uncursed Silver Key" in names, names
        assert "an uncursed small universal key" in names, names

        # Carried energy regeneration fires on a deliberately non-native tick.
        before, after, _ = read_passage(game, necro, "c")
        assert before[0] - 5 < after[0] < before[0], (before, after)
        wait_before = values(game)
        game.send(".", 1)
        game.settle()
        wait_after = values(game)
        assert wait_after[2] == wait_before[2] + 1
        assert wait_after[0] > wait_before[0], (wait_before, wait_after)

        # With no real Step 10C identities, invocation consumes a normal turn
        # but neither assigns age/cooldown nor mutates any destination.
        artifacts, _ = find_inventory(game)
        age_before = artifacts["The Silver Key"][1]
        pos_before = game.state()[:2]
        raw_start = len(game.raw)
        game.send("#invoke\n")
        game.wait("What do you want to invoke?")
        game.send(silver, 1)
        game.settle()
        invoked = tty_transcript("".join(game.raw[raw_start:])) + "\n" + game.text()
        assert "find no door it can open" in invoked, invoked
        artifacts, _ = find_inventory(game)
        assert artifacts["The Silver Key"][1] == age_before, artifacts
        assert game.state()[:2] == pos_before

        game.send("d")
        game.wait("What do you want to drop?")
        game.send(silver, 1)
        game.settle()
        removed = attributes(game, "silver-key-dropped")
        assert "teleport control" not in removed
        assert "polymorph control" not in removed

        # On another non-native tick, energy stays unchanged without the Key.
        before, after, _ = read_passage(game, necro, "c")
        assert before[0] - after[0] == 5, (before, after)
        wait_before = values(game)
        game.send(".", 1)
        game.settle()
        wait_after = values(game)
        assert wait_after[0] == wait_before[0], (wait_before, wait_after)

        # Confusion blocks the menu through the real read entry and consumes a
        # normal turn.  It does not add any persistent occult state.
        # Use the table constructor so a shuffled potion appearance cannot be
        # parsed as the true name on one architecture/seed.
        text = game.lua('''local o=obj.new({id="potion of confusion",class="!"})
local t=o:totable();assert(t.otyp_name=="confusion")
u.giveobj(o);nh.pline("B33_CONFUSION "..o:totable().invlet)''')
        confusion = re.findall(r"B33_CONFUSION (.)", text)[-1]
        game.send("q")
        game.wait("What do you want to drink?")
        game.send(confusion, 1)
        game.settle()
        before = values(game)
        raw_start = len(game.raw)
        game.send("r")
        game.wait("What do you want to read?")
        game.send(necro, 1)
        game.settle()
        confused_output = tty_transcript("".join(game.raw[raw_start:]))
        after = values(game)
        assert "tangled passages make no sense" in confused_output, confused_output
        assert "Which passage will you read?" not in confused_output
        assert after[2] == before[2] + 1

        # Pick the Silver Key back up, then save and reload both artifacts.
        game.send(",", 1)
        game.settle()
        game.save()
        game.close()
        game = Game(release, output, restore=True)
        artifacts, bases = find_inventory(game)
        assert set(("The Necronomicon", "The Silver Key")) <= set(artifacts)
        assert "secrets" in bases
        assert "universal key" in bases
        restored = attributes(game, "silver-key-restored")
        assert "teleport control" in restored
        assert "polymorph control" in restored
        names = inventory_names(game, "identified-artifacts-restored")
        assert "the uncursed Silver Key" in names, names

        print("PASS B3-3 packaged Necronomicon menu, base isolation, Silver Key passives/no-destination, and save/restore",
              flush=True)
    finally:
        game.close()


if __name__ == "__main__":
    main()
