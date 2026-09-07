-- Step 6B runtime checks, 2026-09-06. Load with #wizloadlua on generated
-- DoD floors or inside the Temple of Moloch. Uses existing wizard/Lua APIs;
-- no production test hooks. Population details can be checked separately
-- with monster detection and #wizborn.
local stairs = nh.stairways();
local xoffset, yoffset = nh.abscoord(0,0);
local maps, counts = {}, {};
local traps, thrones, gold, scales, coffers = 0, 0, 0, 0, 0;

local function inspect(o)
   while not o:isnull() do
      local t = o:totable();
      local name = t.otyp_name or "";
      counts[name] = (counts[name] or 0) + 1;
      if name == "gold piece" then gold = gold + t.quan; end
      if string.find(name, "dragon scales") then scales = scales + 1; end
      if name == "chest" and t.spe == 2 then coffers = coffers + 1; end
      if t.has_contents ~= 0 then inspect(o:contents()); end
      o = o:next(true);
   end
end

for x = 1,79 do
   for y = 0,20 do
      local m = nh.getmap(x-xoffset,y-yoffset);
      maps[y*80+x] = m;
      if m.typ_name == "throne" then thrones = thrones + 1; end
      if m.has_trap then traps = traps + 1; end
      inspect(obj.at(x-xoffset,y-yoffset));
   end
   collectgarbage("collect");
end

local function passable(x,y)
   if x < 1 or x > 79 or y < 0 or y > 20 then return false; end
   for _,s in ipairs(stairs) do
      if s.x == x and s.y == y then return true; end
   end
   local c = maps[y*80+x].mapchr;
   return c == "." or c == "#" or c == "+" or c == "S"
       or c == "<" or c == ">" or c == "x" or c == "K" or c == "H"
       or c == "\\" or c == "_";
end

assert(#stairs > 0, "generated floor has no stairway");
local seen, todo = {}, { {stairs[1].x, stairs[1].y} };
while #todo > 0 do
   local p = table.remove(todo);
   local key = p[1]..","..p[2];
   if not seen[key] and passable(p[1],p[2]) then
      seen[key] = true;
      for _,d in ipairs({{1,0},{-1,0},{0,1},{0,-1}}) do
         table.insert(todo,{p[1]+d[1],p[2]+d[2]});
      end
   end
end
for _,s in ipairs(stairs) do
   assert(seen[s.x..","..s.y], "disconnected stair");
end

if u.dnum == 0 and u.dlevel >= 30 and u.dlevel <= 199 then
   assert(#stairs >= 2, "DoD floor is missing its normal stairs");
elseif u.dnum ~= 0 then
   assert(nh.dnum_name(u.dnum) == "The Temple of Moloch",
          "unexpected non-DoD test floor");
   assert(#stairs == 1 and stairs[1].dnum == 0 and stairs[1].up,
          "Temple should expose one return stair");
   assert((counts.chest or 0) == 9, "Temple treasure chests");
   assert((counts["wax candle"] or 0) >= 8, "Temple candles");
end

nh.pline(string.format("STEP6B PASS %d:%d stairs=%d throne=%d coffers=%d gold=%d scales=%d traps=%d",
    u.dnum, u.dlevel, #stairs, thrones, coffers, gold, scales, traps));
maps = nil;
collectgarbage("collect");
