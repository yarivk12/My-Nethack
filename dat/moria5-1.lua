-- Ruins of the Dwarrowdelf, by Guest41. NetHack license.
-- UnNetHack 439b8d63d3d1ca78fb08588dd43f61874114b21a:dat/moria.des (moria5-1).
-- Step 8A Lua port; runtime dependencies documented in doc/step8a.md.
des.level_init({ style="solidfill", fg=" " })
des.level_flags("hardfloor", "mazelevel")
des.map([[
    |....................................................................|..
    |................................................................{{..|..
    |....................................................................|..
    |..................................................................---F-
   --..................................................................|....
   |...................................................................|....
   |...................................................................|....
-----..............................................................------S--
|...|..............................................................|........
|...S..............................................................S........
|...|..............................................................|........
-----..............................................................------S--
   |...................................................................|....
   |...................................................................|....
   --..................................................................|....
    |..................................................................---F-
    |....................................................................|..
    |................................................................{{..|..
    |....................................................................|..
]])

-- Outdoor sky and monster-generation override are derived from level identity.
des.region(selection.area(1,1,74,17), "lit")
des.non_diggable(selection.area(0,0,75,18))
des.stair("down", 2,9)
des.stair("up", 72,9)
local live, dead = 5, 20
if percent(75) then live, dead = 20, 5 end
des.replace_terrain({ region={6,0,65,18}, fromterrain=".", toterrain="T", chance=live })
des.replace_terrain({ region={6,0,65,18}, fromterrain=".", toterrain="t", chance=dead })
if percent(25) then
    des.replace_terrain({ region={6,0,65,18}, fromterrain=".", toterrain="M", chance=20 })
    for i = 1, d(4) do des.monster("swamp fern") end
end
des.monster({ id="iron golem", coord=selection.area(6,0,65,18):rndcoord(), peaceful=true,
    inventory=function()
        des.object({ id="axe", buc="uncursed", spe=d(4) })
        if percent(50) then des.object("tin opener") end
        if percent(20) then des.object("can of grease") end
        if percent(15) then des.object("tinning kit") end
    end })
for i = 1, d(2,4) do des.trap("bear") end
for i = 1, d(2,6) do des.object() end
for i = 1, d(2,8) do des.object("%") end
-- Barracks deliberately leave peacefulness to the ordinary species rules.
for _, y in ipairs({4,5,6,12,13,14}) do
    for x = 72, 75 do des.monster("deep orc", x,y) end
end
if nh.night() then
    des.monster("bat")
    des.monster("bat")
    des.monster("B")
    if percent(50) then des.monster("B") end
end
for _, y in ipairs({0,1,2,16,17,18}) do
    for x = 74, 75 do des.monster({ id="dwarf", coord={x,y}, peaceful=true }) end
end

-- Branch flavor: ordinary cursed teleportation scrolls with a custom name.
for i = 1, d(2) do
    des.object({ id="scroll of teleportation", buc="cursed", name="Word of Recall" })
end
