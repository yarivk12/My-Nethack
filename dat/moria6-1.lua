-- Ruins of the Dwarrowdelf, by Guest41. NetHack license.
-- UnNetHack 439b8d63d3d1ca78fb08588dd43f61874114b21a:dat/moria.des (moria6-1).
-- Step 8A Lua port; runtime dependencies documented in doc/step8a.md.
des.level_init({ style="solidfill", fg=" " })
des.level_flags("graveyard", "hardfloor", "mazelevel")
des.map([[
-----FFF--------------------------------------...........}}}}}}}}}}}}}
|...........|.|.|.|.|...............|........|..........}}}}}}}}}}}}}}}}}}
|-..........|.......|...............|-......-|.....T....}}}}}}}}}}}}}}}}}}}}
|...........+.......|...............|........|...........}}}}}}}}}}}}}}}}}}}
|-..........+.......S...............|-......-|...T........}}}}}}}}}}}}}}}}}}
|...........|.......|...............|........|............}}}}}}}}}}}}}}}}}
|-..........|.......|...............|----+---|.......T...}}}}}}}}}}}}}}}}}}
|..|.|.|.|..|.|.|.|.|...............S........|...........}}}}}}}}}}}}}}}}}}
|--------------------------+------------------...........}}}}}}}}}}}}}}}}}}}
|..........................................|............}}}}}}}}}}}}}}}}}}}}
|..........................................S............}}}}}}}}}}}}}}}}}}}
|.....{....................................|............}}}}}}}}}}}}}}}}}}}
|...........---------------+------------------...........}}}}}}}}}}}}}}}}}}}
|...........|....|...........................|...........}}}}}}}}}}}}}}}}}}}
|...........|....|...........................|.......T...}}}}}}}}}}}}}}}}}}}
|--FF---FF--|....|...........................|............}}}}}}}}}}}}}}}}}
|...........|....|...........................|...T........}}}}}}}}}}}}}}}}}
|...........S....S...........................|...........}}}}}}}}}}}}}}}}}}
|...........|....|...........................|.....T....}}}}}}}}}}}}}}}}}}
----------------------------------------------.........}}}}}}}}}}}}}}}}}}
]])

-- Outdoor sky, ambient waters and monster-generation override are in C.
des.region(selection.area(1,1,75,20), "lit")
des.teleport_region({ region={1,16,11,18} })
des.non_diggable(selection.area(0,0,75,20))
des.stair("down", 4,17)
des.region({ region={13,13,16,18}, lit=0, type="morgue", filled=1 })

for _, coord in ipairs({{12,3},{12,4},{27,8},{27,12},{41,6}}) do
    des.door({ state="closed", coord=coord })
end
des.door("locked", 20,4)
des.object({ id="rock", coord={30,4}, quantity=21 })
des.object("flint", 30,4)
des.object("lembas wafer", 30,4)

des.grave({ coord={6,3}, text="Balin, Son of Fundin, Lord of Moria" })
for _, x in ipairs({1,2,10,11}) do
    des.object({ id="statue", coord={x,7}, montype="dwarf", historic=true })
end
local mazarbul = {{1,1},{1,3},{1,5},{4,7},{6,7},{8,7}}
shuffle(mazarbul)
local classes = {"?", "+", "!", "/", '"', "="}
for i, coord in ipairs(mazarbul) do
    des.object({ id="chest", coord=coord,
                 contents=function() des.object(classes[i]) end })
end
for _, coord in ipairs({{13,1},{15,1},{17,1},{19,1},{13,7},{15,7},{17,7},{19,7}}) do
    des.object({ id="chest", coord=coord,
                 contents=function() des.object({ quantity=d(3) }) end })
end
for i = 1, 5+d(2,4) do
    des.object({ class="(", coord=selection.area(18,13,44,18):rndcoord() })
end

-- Branch flavor: ordinary cursed teleportation scrolls with a custom name.
for i = 1, d(2) do
    des.object({ id="scroll of teleportation", buc="cursed", name="Word of Recall" })
end
des.altar({ coord={41,3}, align="noncoaligned", type="altar" })
local altars = {{44,1},{44,3},{44,5},{37,1},{37,3},{37,5}}
shuffle(altars)
for i, id in ipairs({"ice box","iron safe","iron safe","large box","large box","large box"}) do
    des.object({ id=id, coord=altars[i] })
end
if nh.night() then
    des.engraving({ coord={44,10}, type="engrave", text="Speak, friend, and enter." })
end
local mellon = {{44,9},{44,10},{44,11},{45,9},{45,10},{45,11}}
shuffle(mellon)
des.object({ id="melon", coord=mellon[1], quantity=1 })
des.monster({ id="Watcher in the Water", coord={66,10}, inventory=function()
    des.object({ id="magic lamp", buc="uncursed" })
end })
