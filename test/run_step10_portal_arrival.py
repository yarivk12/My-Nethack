"""Actual destination-aware arrival regression for the Neutral portal chain.

This runs the packaged wizard game and traverses the complete Gate/outlands/
Spire chain in both directions.  It deliberately chooses the non-arrival
portal for each two-portal floor, so an arrival on the wrong portal changes
the next expected destination and fails the test.
"""
from pathlib import Path
import re
import sys

from run_step8a_runtime import Game


def settle_portal(game):
    """Advance portal dizziness without changing the arrival level/point."""
    before = game.state()
    game.send("m.", 1)
    game.settle()
    assert game.state() == before, (before, game.state(), game.text())


def clear_monster(game, target):
    """Clear a late-moving blocker without advancing the game clock."""
    if not monster_occupied(game, target):
        return
    x, y = game.state()[2:]
    tx, ty = target
    keys = (("l" if tx > x else "h") * abs(tx - x)
            + ("j" if ty > y else "k") * abs(ty - y))
    game.send("#WIZKILL\n")
    game.wait("Pick first monster to slay")
    game.settle()
    game.send(keys + ".")
    game.settle()
    game.send("\x1b")
    game.settle()


def monster_occupied(game, target):
    """Check a live map cell before issuing the wizard targeting command."""
    tx, ty = target
    text = game.lua(
        """local ox,oy=nh.abscoord(0,0)
local m=nh.getmap(%d-ox,%d-oy)
nh.pline("MON_OCC " .. (m.glyph <= 9*nhc.NUMMONS and "M" or "-"))
""" % (tx, ty)
    )
    return re.findall(r"MON_OCC ([M-])", text)[-1] == "M"


def portal_approach(game, exclude=None):
    """Return one portal and a walkable adjacent square, excluding a point."""
    excluded = "false" if exclude is None else "(q.x==%d and q.y==%d)" % exclude
    text = game.lua(
        """local ps={};local ox,oy=nh.abscoord(0,0)
for x=1,79 do for y=0,20 do
 local m=nh.getmap(x-ox,y-oy)
 if m.has_trap then
  local t=nh.gettrap(x-ox,y-oy)
  if t.ttyp_name==\"magic portal\" then ps[#ps+1]={x=x,y=y} end
 end
end end
local p=nil
for _,q in ipairs(ps) do if not EXCLUDED then p=q;break end end
assert(p,\"missing requested portal\")
local spot=nil
for _,d in ipairs({{-1,0},{1,0},{0,-1},{0,1},{-1,-1},{1,-1},{-1,1},{1,1}}) do
 local x,y=p.x+d[1],p.y+d[2]
 if x>=1 and x<80 and y>=0 and y<21 then
  local m=nh.getmap(x-ox,y-oy)
  if string.find(\".QsGeI#\",m.mapchr,1,true) and not m.has_trap
     and m.glyph > 9*nhc.NUMMONS then
   spot={x=x,y=y};break
  end
 end
end
assert(spot,\"no walkable portal approach\")
local pm=nh.getmap(p.x-ox,p.y-oy)
nh.pline(string.format(\"PORTAL %d %d %d %d %s %s\",p.x,p.y,spot.x,spot.y,
                       pm.mapchr,pm.glyph <= 9*nhc.NUMMONS and \"M\" or \"-\"))
""".replace("EXCLUDED", excluded)
    )
    px, py, sx, sy, _mapchr, occupied = re.findall(
        r"PORTAL (\d+) (\d+) (\d+) (\d+) (.) ([M-])", text
    )[-1]
    return (int(px), int(py)), (int(sx), int(sy)), occupied == "M"


def reveal_level(game):
    """Reveal the current level so controlled teleport can target scan results."""
    game.send("\x06", 1)  # wizard-mode ^F / #wizmap
    game.settle()


def portal_occupied(game, portal):
    """Recheck occupancy after controlled teleporting and monster movement."""
    px, py = portal
    text = game.lua(
        """local ox,oy=nh.abscoord(0,0)
local m=nh.getmap(%d-ox,%d-oy)
nh.pline("PORTAL_OCC " .. (m.glyph <= 9*nhc.NUMMONS and "M" or "-"))
""" % (px, py)
    )
    return re.findall(r"PORTAL_OCC ([M-])", text)[-1] == "M"


def move_to(game, target):
    tx, ty = target
    if game.state()[2:] == (tx, ty):
        return
    for attempt in range(4):
        x, y = game.state()[2:]
        game.send("\x14")
        game.wait("Where do you want to be teleported?")
        game.settle()
        game.send(("l" if tx > x else "h") * abs(tx - x)
                  + ("j" if ty > y else "k") * abs(ty - y) + ".")
        game.settle()
        if game.state()[2:] == target:
            return
        # The target was selected from a live map scan, but a monster can
        # occupy it during the teleport prompt.  Clear and re-target after
        # every miss; the outer bound keeps a pathological fixture finite.
        clear_monster(game, target)
    assert game.state()[2:] == target, game.text()


def enter_portal(game, expected, exclude=None):
    # A map scan and the controlled teleport are separated by several game
    # turns.  Re-scan after a failed relocation so a newly occupied approach
    # square or portal cannot invalidate the rest of this entry attempt.
    for relocation in range(4):
        reveal_level(game)
        portal, approach, occupied = portal_approach(game, exclude)
        try:
            move_to(game, approach)
            break
        except AssertionError:
            if relocation == 3:
                raise
    dx, dy = portal[0] - approach[0], portal[1] - approach[1]
    keys = {(-1, -1): "y", (0, -1): "k", (1, -1): "u",
            (-1, 0): "h", (1, 0): "l", (-1, 1): "b",
            (0, 1): "j", (1, 1): "n"}
    # The controlled teleport consumes a turn.  A monster can move onto the
    # portal after the initial scan, so recheck immediately before stepping.
    for attempt in range(3):
        if occupied or portal_occupied(game, portal):
            # The probe itself consumes a turn, so a monster can move onto
            # the portal after the first scan. Clear it, then re-teleport to
            # the approach square before trying the portal again.
            clear_monster(game, portal)
            move_to(game, approach)
        occupied = False
        game.send(keys[(dx, dy)], 1)
        game.settle()
        if "Really step into that magic portal?" in game.text():
            game.send("y", 1)
            game.settle()
        actual = game.state()
        if actual[:2] == expected:
            break
        # A late monster move can block the step after the occupancy probe.
        # Some monster-on-portal redraws briefly report the trap as empty even
        # though the attempted step attacked the blocker.  An unchanged
        # approach position is therefore sufficient evidence for the bounded
        # cleanup/retry path.
        if actual[2:] == approach or portal_occupied(game, portal):
            clear_monster(game, portal)
            move_to(game, approach)
            continue
        assert actual[:2] == expected, (expected, actual, game.text())
    assert actual[:2] == expected, (expected, actual, game.text())
    settle_portal(game)
    assert game.state() == actual, (expected, actual, game.state())
    print("PASS portal arrival", actual[:2], "at", actual[2:], flush=True)
    return actual[2:]


def branch_parent(game):
    game.send("#WIZWHERE\n", 1)
    text = game.wait("Portal to Neutral Quest:", more=False)
    matches = re.findall(r"Portal to Neutral Quest: (\d+)", text)
    assert len(matches) == 1, text
    game.send("\x1b")
    game.settle()
    return int(matches[0])


def main():
    release, output = sys.argv[1:3]
    game = Game(release, output)
    try:
        game.settle()
        game.send("#LEVELCHANGE\n")
        game.wait("To what experience level")
        game.send("30\n")
        game.settle()
        # Keep the traversal fixture focused on portal semantics.  The
        # established debug flag removes the current level's monsters and
        # suppresses future random generation, preventing combat turns from
        # invalidating otherwise valid portal targets.
        game.lua("nh.debug_flags({hunger=false,mongen=false})")

        parent = branch_parent(game)
        neutral = game.dungeon_number("Neutral Quest")
        game.send("\x16")
        game.wait("To what level")
        game.send(str(parent) + "\n", 1)
        game.settle()
        assert game.state()[:2] == (0, parent)

        # DoD approach -> Gate Town, then the complete forward chain.
        gate_arrival = enter_portal(game, (neutral, 1))
        forward = [("out1", 2), ("out2", 3), ("out3", 4),
                   ("out4", 5), ("spire", 6)]
        arrivals = {1: gate_arrival}
        for name, level in forward:
            del name
            arrivals[level] = enter_portal(game, (neutral, level),
                                            arrivals[level - 1])
            if level == 3:
                before = game.state()
                game.save()
                game = Game(release, output, restore=True)
                assert game.state() == before, (before, game.state())
                print("PASS portal save/reload", before[:2], flush=True)

        # Spire -> Gate Town, then reverse every paired edge to the DoD
        # approach.  The non-arrival choice is the forward/reverse oracle.
        for level in range(5, 0, -1):
            # Spire has only its return portal; lower floors have paired
            # portals, so avoid the portal used for the forward arrival.
            exclude = None if level == 5 else arrivals[level]
            arrivals[level - 1] = enter_portal(
                game, (neutral, level), exclude
            )
        enter_portal(game, (0, parent), arrivals[0])
        assert game.state()[:2] == (0, parent)
        print("PASS full Neutral Gate/out1/out2/out3/out4/spire forward/reverse",
              flush=True)
    finally:
        game.close()


if __name__ == "__main__":
    main()
