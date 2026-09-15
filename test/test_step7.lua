-- Step 7: #wizloadlua in a fresh classic Tomb, and again after save/reload.
-- Uses existing wizard APIs; does not modify the floor or inventory.
assert(nh.dnum_name(u.dnum) == "The Lost Tomb", "not in Lost Tomb");
assert(u.dlevel == 1, "Tomb must have one level");
local stairs = nh.stairways();
assert(#stairs == 1 and stairs[1].up and stairs[1].dnum == 0,
       "Tomb needs exactly one return stair to DoD");
assert(stairs[1].dlevel >= 30 and stairs[1].dlevel <= 199,
       "Tomb return depth outside DL30-199");
local ox, oy = nh.abscoord(0,0);
local chests, locked, trapped, wax, pits = 0,0,0,0,0;
local function inspect(o)
   while not o:isnull() do
      local t = o:totable();
      if t.otyp_name == "chest" then
         chests = chests + 1;
         if t.olocked ~= 0 then locked = locked + 1; end
         if t.otrapped ~= 0 then trapped = trapped + 1; end
      elseif t.otyp_name == "wax candle" then wax = wax + t.quan; end
      if t.has_contents ~= 0 then inspect(o:contents()); end
      o = o:next();
   end
end
-- Walk the global floor chain once, including each container's contents.
-- Avoid allocating an object userdata for every empty map square.
inspect(obj.next());
for x=1,79 do
   for y=0,20 do
      local m = nh.getmap(x-ox,y-oy);
      if m.has_trap then
         local t = nh.gettrap(x-ox,y-oy);
         if t.ttyp_name == "spiked pit" then pits = pits + 1; end
      end
   end
end
assert(chests == 8, "eight donor chests");
assert(locked >= 4 and trapped >= 1, "locked/trapped donor reward chests");
assert(wax >= 3, "three wax candle caches");
assert(pits == 5, "five donor spiked pits");
nh.pline(string.format("STEP7 TOMB PASS return=DoD%d chests=%d locked=%d trapped=%d wax=%d pits=%d",
    stairs[1].dlevel,chests,locked,trapped,wax,pits));
