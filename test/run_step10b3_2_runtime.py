"""Exercise the eight B3-2 artifacts and the live Neutrality-key births."""
import json
import re
import sys
import time
from pathlib import Path

import run_step8a_runtime as runtime_harness
from run_step8a_runtime import Game


# The Windows TTY port's printable-input path is not compatible with the
# current WinPTY pseudo-terminal after an extended-command prompt: only the
# initiator reaches ReadConsoleInput.  Exercise the same packaged binary via
# its bundled curses window port, which preserves the real save/object/Lua
# lifecycle while keeping this test independent of a native GUI window.
_spawn = runtime_harness.PtyProcess.spawn


def _spawn_curses(*args, **kwargs):
    env = dict(kwargs.get("env", {}))
    env["NETHACKOPTIONS"] = env.get("NETHACKOPTIONS", "").replace(
        "windowtype:tty", "windowtype:curses")
    kwargs["env"] = env
    return _spawn(*args, **kwargs)


runtime_harness.PtyProcess.spawn = _spawn_curses


def _settle_curses(game):
    """Drain curses pages without relying on the TTY screen model."""
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
    """Persist the fixture, then terminate the curses wrapper cleanly."""
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
key_output = output.with_name(output.name + "-keys")

ARTIFACTS = [
    "The First Key of Neutrality",
    "The Second Key of Neutrality",
    "The Third Key of Neutrality",
    "Infinity's Mirrored Arc",
    "The Staff of Twelve Mirrors",
    "The Sansara Mirror",
    "Mirror Brand",
    "Soulmirror",
]


def attributes(game, label):
    game.send("#attributes\n", 1)
    game.wait("Wizard's attributes:", more=False)
    # PDCurses renders the complete text window without the TTY page
    # counters used by the shared harness.
    time.sleep(1)
    text = game.text()
    (game.path / (label + ".txt")).write_text(text, encoding="utf8")
    game.send("\x1b")
    game.settle()
    return text


def give(game, name):
    encoded = json.dumps("uncursed " + name)
    expected = json.dumps(name)
    text = game.lua(
        "local o=obj.new(%s);local t=o:totable();"
        "assert(t.oname==%s,'wrong artifact name '..tostring(t.oname));"
        "u.giveobj(o);nh.pline('ART_ITEM '..o:totable().invlet..' '.."
        "o:totable().oname)" % (encoded, expected))
    found = re.findall(r"ART_ITEM (.) (.*?)(?:LUA_DONE_\d+|\r?\n|$)", text)
    found = [(letter, actual.strip()) for letter, actual in found]
    assert found and found[-1][1] == name, text
    return found[-1][0]


def inventory_names(game):
    text = game.lua('''local o=u.inventory;local n=0
while not o:isnull() do
 local t=o:totable();if t.has_oname~=0 then
  n=n+1;nh.pline("ART_NAME "..t.oname.." "..t.usecount)
 end
 o=o:next()
end
nh.pline("ART_TOTAL "..n)''')
    names = re.findall(r"ART_NAME (.+?) (\d+)", text)
    total = int(re.findall(r"ART_TOTAL (\d+)", text)[-1])
    return names, total


def equip(game, command, letter, prompt):
    game.send(command)
    game.wait(prompt)
    game.send(letter, 1)
    game.settle()


def artifact_runtime():
    game = Game(release, output)
    try:
        game.lua("nh.debug_flags({hunger=false,mongen=false});u.clear_inventory()")
        letters = {name: give(game, name) for name in ARTIFACTS}
        names, total = inventory_names(game)
        assert total == 8, names
        assert {name for name, _ in names} == set(ARTIFACTS), names
        assert next(int(value) for name, value in names
                    if name == "Infinity's Mirrored Arc") == 0

        arc = letters["Infinity's Mirrored Arc"]
        game.send("#invoke\n")
        game.wait("What do you want to invoke?")
        game.send(arc)
        game.settle()
        assert "second beam-path" in game.text(), game.text()
        names, _ = inventory_names(game)
        assert next(int(value) for name, value in names
                    if name == "Infinity's Mirrored Arc") == 1

        staff = letters["The Staff of Twelve Mirrors"]
        equip(game, "w", staff, "What do you want to wield?")
        staff_attrs = attributes(game, "staff-of-twelve-mirrors")
        assert "reflection" in staff_attrs.lower()
        assert "displaced" in staff_attrs.lower()

        sansara = letters["The Sansara Mirror"]
        equip(game, "w", sansara, "What do you want to wield?")
        sansara_attrs = attributes(game, "sansara-mirror")
        assert "reflection" in sansara_attrs.lower()
        assert "half spell damage" in sansara_attrs.lower()

        soulmirror = letters["Soulmirror"]
        equip(game, "W", soulmirror, "What do you want to wear?")
        soul_attrs = attributes(game, "soulmirror")
        assert "reflection" in soul_attrs.lower()
        assert "level-drain resistant" in soul_attrs.lower()

        game.save()
        game.close()
        game = Game(release, output, restore=True)
        names, total = inventory_names(game)
        assert total == 8 and {name for name, _ in names} == set(ARTIFACTS), names
        assert next(int(value) for name, value in names
                    if name == "Infinity's Mirrored Arc") == 1
        print("PASS B3-2 runtime artifact naming, bounded properties, alt-mode and save/restore",
              flush=True)
    finally:
        game.close()


def teleport_to(game, tx, ty):
    x, y = game.state()[2:]
    game.send("\x14")
    game.wait("Where do you want to be teleported?")
    game.settle()
    game.send(("l" if tx > x else "h") * abs(tx - x)
              + ("j" if ty > y else "k") * abs(ty - y) + ".")
    game.settle()
    assert game.state()[2:] == (tx, ty), game.text()


def dropped_names(game):
    text = game.lua('''local seen={}
for x=1,12 do for y=1,6 do
 local o=obj.at(x,y)
 while not o:isnull() do
  local t=o:totable();if t.has_oname~=0 then
   seen[t.oname]=(seen[t.oname] or 0)+t.quan
  elseif t.otyp_name=="universal key" then
   seen["universal key"]=(seen["universal key"] or 0)+t.quan
  end
  o=o:next(true)
 end
end end
for k,v in pairs(seen) do nh.pline("DROP "..k.." "..v) end''')
    return dict(re.findall(r"DROP (.+?) (\d+)", text))


def key_runtime():
    game = Game(release, key_output)
    try:
        game.lua("nh.debug_flags({hunger=false,mongen=false});"
                  "nh.debug_flags({mongen=true})")
        fixture = '''des.level_init({style="solidfill",fg=" "})
des.level_flags("noflip","nomongen")
des.map({x=1,y=0,map=[=[
------------
|............|
|............|
|............|
|............|
|............|
------------
]=],contents=function(rm)
 des.monster({id="Center of All",coord={10,3},asleep=true,paralyzed=126,cancelled=true,peaceful=false})
 des.monster({id="Alhoon",coord={3,3},asleep=true,paralyzed=126,cancelled=true,peaceful=false})
 des.monster({id="Alhoon",coord={4,3},asleep=true,paralyzed=126,cancelled=true,peaceful=false})
 des.monster({id="Alhoon",coord={5,3},asleep=true,paralyzed=126,cancelled=true,peaceful=false})
 des.monster({id="Alhoon",coord={6,3},asleep=true,paralyzed=126,cancelled=true,peaceful=false})
end})
des.region(selection.area(1,1,12,5),"lit")
'''
        (game.path / "keys.lua").write_text(fixture, encoding="utf8")
        game.send("#wizloaddes\n")
        game.wait("Load which des lua file?")
        game.send("keys.lua\n", 2)
        game.settle()

        teleport_to(game, 10, 3)
        game.send("#wizkill\n")
        game.wait("Pick first monster to slay")
        for cursor_move in ("l", "hhhhhhh", "l", "l", "l"):
            game.send(cursor_move + ".", 2)
            game.wait("Next monster")
        game.send("\x1b")
        game.settle()

        drops = dropped_names(game)
        assert drops.get("The First Key of Neutrality") == "1", drops
        assert drops.get("The Second Key of Neutrality") == "1", drops
        assert drops.get("The Third Key of Neutrality") == "1", drops
        assert sum(int(v) for k, v in drops.items()
                   if k == "universal key") >= 2, drops
        print("PASS Center First Key and four-Alhoon Second->Third->ordinary births",
              flush=True)
    finally:
        game.close()


if __name__ == "__main__":
    artifact_runtime()
    key_runtime()
