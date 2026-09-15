-- Ruins of the Dwarrowdelf, by Guest41. NetHack license.
-- UnNetHack 439b8d63d3d1ca78fb08588dd43f61874114b21a:dat/moria.des (moria3-1).
-- Step 8A Lua port; runtime dependencies documented in doc/step8a.md.
des.level_init({ style="solidfill", fg=" " })
des.level_flags("hardfloor", "mazelevel")

-- Regeneration, monster migration, glyphs and ambient feel message are in C.
des.message("The dungeon here seems less persistent.")
des.room({ type="ordinary", contents=function() des.stair("up") end })
des.room({ type="ordinary", contents=function() des.stair("down") end })
des.room({ type="ordinary", contents=function()
    if percent(75) then
        des.engraving({ type="engrave", text="Why, oh why didn't I leave my items in the town?" })
    end
end })
for i = 1, 4 do des.room({ type="ordinary" }) end
des.random_corridors()
