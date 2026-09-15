-- Ruins of the Dwarrowdelf, by Guest41. NetHack license.
-- UnNetHack 439b8d63d3d1ca78fb08588dd43f61874114b21a:dat/moria.des (moria1-1).
-- Step 8A Lua port; runtime dependencies documented in doc/step8a.md.
des.level_init({ style="solidfill", fg=" " })
des.level_flags("hardfloor", "mazelevel", "noteleport")
des.map([[
...........................................................................
...........................................................................
...........................................................................
...........................................................................
...........................................................................
...........................................................................
...........................................................................
...........................................................................
...........................................................................
...........................................................................
...........................................................................
............------FFF-----FFF-----FFF-----FFF-----FFF-----FFF-----FFF------
............|.............................................................|
............|.............................................................|
............F.............................................................|
............|.............................................................|
............|.............................................................|
............|.............................................................|
............S.............................................................|
............---------------------------------------------------------------
]])

des.region(selection.area(1,1,73,18), "lit")
des.teleport_region({ region={15,12,70,18} })
des.levregion({ region={43,15,43,15}, type="branch" })
des.stair("up", 71,2)
des.engraving({ coord={12,18}, type="burn",
    text="Herein lie the lower remnants of the Endless Stair." })
-- Legacy '&' is union, not intersection (lev_comp.y: SPO_SEL_ADD).
local gemarea = selection.area(3,3,9,15) | selection.area(13,3,65,8)
for i = 1, 20 do
    local center = gemarea:rndcoord()
    local pile = selection.circle(center.x, center.y, 2, 1)
    for j = 1, 10 do
        des.object({ class="*", coord=pile:rndcoord() })
        des.terrain({ coord=pile:rndcoord(), typ="L" })
    end
end

-- Branch flavor: ordinary cursed teleportation scrolls with a custom name.
for i = 1, d(2) do
    des.object({ id="scroll of teleportation", buc="cursed", name="Word of Recall" })
end

for i = 1, d(2,8) do
    des.object({ id="small piece of unrefined mithril", buried=true,
                 coord=selection.area(15,12,70,18):rndcoord() })
end
