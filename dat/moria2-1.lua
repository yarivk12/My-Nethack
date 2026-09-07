-- Ruins of the Dwarrowdelf, by Guest41. NetHack license.
-- UnNetHack 439b8d63d3d1ca78fb08588dd43f61874114b21a:dat/moria.des (moria2-1).
-- Step 8A Lua port; runtime dependencies documented in doc/step8a.md.
des.level_init({ style="solidfill", fg=" " })
des.level_flags("mazelevel", "noflipx", "noteleport")
des.map([[
---------------------------------------------------------------------------
|..........LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL|
|..........LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL.|
|..........LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL..|
|..........LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL...|
|..........LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL..---
------.....LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL..---
    -----..LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL..---
 ------.....LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL...------
 |........................................................................|
 ------.....LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL...------
  ------...LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL..---
----.......LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL..---
|..........LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL..---
|..........LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL..|
|..........LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL.|
|..........LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL|
|..........LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL|
---------------------------------------------------------------------------
]])

des.region(selection.area(1,1,73,18), "lit")
des.teleport_region({ region={1,1,10,17} })
des.non_diggable(selection.area(0,0,74,17))
des.stair("up", 73,9)
des.stair("down", 2,9)
des.door("open", 4,9)
des.door("open", 71,9)
des.trap("hole", 35,9)
des.trap("hole", 36,9)
if percent(20) then
    des.replace_terrain({ region={11,1,75,17}, fromterrain="L", toterrain=".", chance=5 })
end
des.object({ id="corpse", coord={37,9}, montype="wizard" })
des.object({ id="quarterstaff", coord={37,9}, eroded=2 })
des.object({ id="robe", coord={37,9}, eroded=1 })
des.monster({ id="Durin's Bane", coord={66,9}, peaceful=false, asleep=false,
    inventory=function()
        des.object({ id="bullwhip", buc="uncursed", eroded=-1, spe=d(7) })
        if percent(33) then des.object("shield of reflection") end
        des.object("wand of speed monster")
        des.object({ id="potion of paralysis", buc="cursed" })
    end })
for i = 1, d(2,5) do des.object() end
for i = 1, d(2,4) do des.object({ id="corpse", montype="dwarf" }) end
for i = 1, d(2,2) do
    des.object({ id="dwarvish mithril-coat", buried=true, buc="cursed" })
end

-- Branch flavor: ordinary cursed teleportation scrolls with a custom name.
for i = 1, d(2) do
    des.object({ id="scroll of teleportation", buc="cursed", name="Word of Recall" })
end
