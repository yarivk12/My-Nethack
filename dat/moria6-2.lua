-- Ruins of the Dwarrowdelf, by Guest41. NetHack license.
-- UnNetHack 439b8d63d3d1ca78fb08588dd43f61874114b21a:dat/moria.des (moria6-2).
-- Step 8A Lua port; runtime dependencies documented in doc/step8a.md.
des.level_init({ style="solidfill", fg=" " })
des.level_flags("graveyard", "hardfloor", "mazelevel")
des.map([[
-----FFF--------------------------------------...........}}}}}}}}}}}}}
|...........|.|.|.|.|...............|........|..........}}}}}}}}}}}}}}}}}}
|-..........|.......|...............|-......-|.....t....}}}}}}}}}}}}}}}}}}}}
|...........+.......|...............|........|...........}}}}}}}}}}}}}}}}}}}
|-..........+.......+...............|-......-|...t........}}}}}}}}}}}}}}}}}}
|...........|.......|...............|........|............}}}}}}}}}}}}}}}}}
|-..........|.......|...............|----+---|.......t...}}}}}}}}}}}}}}}}}}
|..|.|.|.|..|.|.|.|.|...............S........|...........}}}}}}}}}}}}}}}}}}
|--------------------------+------------------........}}}}}}}}}}}}}}}}}}}}}}
|..........................................|}.}....}}}}}}}}}}}}}}}}}}}}}}}}}
|............................................}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
|.....{....................................|}.}....}}}}}}}}}}}}}}}}}}}}}}}}
|...........---------------+------------------........}}}}}}}}}}}}}}}}}}}}}}
|...........|....|...........................|...........}}}}}}}}}}}}}}}}}}}
|...........|....|...........................|.......t...}}}}}}}}}}}}}}}}}}}
|--FF---FF--|....|...........................|............}}}}}}}}}}}}}}}}}
|...........|....|...........................|...t........}}}}}}}}}}}}}}}}}
|...........S....S...........................|...........}}}}}}}}}}}}}}}}}}
|...........|....|...........................|.....t....}}}}}}}}}}}}}}}}}}
----------------------------------------------.........}}}}}}}}}}}}}}}}}}
]])

-- Outdoor sky, ambient waters and monster-generation override are in C.
des.region(selection.area(1,1,75,20), "lit")
des.teleport_region({ region={1,16,11,18} })
des.non_diggable(selection.area(0,0,75,20))
des.stair("down", 4,17)
des.region({ region={13,13,16,18}, lit=0, type="morgue", filled=1 })

local doors = {{12,3},{12,4},{20,4},{27,8},{27,12},{41,6}}
shuffle(doors)
for i, coord in ipairs(doors) do
    des.door({ state=i<=2 and "closed" or "broken", coord=coord })
end
des.object({ id="boulder", coord=doors[5] })
des.object({ id="boulder", coord=doors[6] })
des.object("boulder", 43,10)
for i = 1, 10+d(2,5) do
    if percent(90) then des.object("boulder") end
    des.object("rock")
end
for _, monster in ipairs({"hobbit","hobbit","hobbit","hobbit","human","dwarf","elf"}) do
    des.object({ id="corpse", coord=selection.area(21,1,35,7):rndcoord(), montype=monster })
end

des.grave({ coord={6,3}, text="Balin, Son of Fundin, Lord of Moria" })
for _, x in ipairs({1,2,10,11}) do
    des.object({ id="rock", coord={x,7}, quantity=d(25) })
end
local mazarbul = {{1,1},{1,3},{1,5},{4,7},{6,7},{8,7}}
shuffle(mazarbul)
local classes = {"?", "+", "!", "/", '"', "="}
for i, coord in ipairs(mazarbul) do
    des.object({ id="chest", coord=coord,
                 contents=function() des.object(classes[i]) end })
end
for _, coord in ipairs({{13,1},{15,1},{17,1},{19,1},{13,7},{15,7},{17,7},{19,7}}) do
    if percent(50) then
    des.object({ id="chest", coord=coord,
                 contents=function() des.object({ quantity=d(3) }) end })
    end
end
for i = 1, 5+d(2,4) do
    des.object({ class="(", coord=selection.area(18,13,44,18):rndcoord() })
end

-- Branch flavor: ordinary cursed teleportation scrolls with a custom name.
for i = 1, d(2) do
    des.object({ id="scroll of teleportation", buc="cursed", name="Word of Recall" })
end
des.object("boulder", 41,3)
local altars = {{44,1},{44,3},{44,5},{37,1},{37,3},{37,5}}
shuffle(altars)
for i, id in ipairs({"ice box","iron safe","iron safe","large box","large box","large box"}) do
    if percent(75) then
    des.object({ id=id, coord=altars[i] })
    end
end
local mellon = {{45,9},{44,10},{45,11}}
shuffle(mellon)
des.object({ id="melon", coord=mellon[1], quantity=1 })
for i = 1, 10+d(2,5) do des.monster({ id="deep orc", peaceful=false }) end
des.monster({ id="Watcher in the Water", coord={66,10}, inventory=function()
    des.object({ id="magic lamp", buc="uncursed" })
end })
