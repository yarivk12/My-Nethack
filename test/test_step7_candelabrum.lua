-- #wizloadlua immediately after attaching seven Magic Candles to an empty
-- Candelabrum, then again after lighting it through normal apply.
local found=false;
local o=u.inventory;
while not o:isnull() do
   local t=o:totable();
   if t.otyp_name=="Candelabrum of Invocation" then
      found=true;
      assert(t.spe==7,"seven attached candles");
      if t.lamplit~=0 then
         assert(o:has_timer("burn-obj"),"Candelabrum must burn finite fuel");
         assert(t.age>=0 and t.age<600,"finite remaining fuel");
      else
         assert(t.age==600,"donor Magic Candle conversion gives 600 turns");
         assert(not o:has_timer("burn-obj"));
      end
      nh.pline("STEP7 CANDELABRUM PASS spe=7 finite fuel, lit="..t.lamplit);
   end
   o=o:next();
end
assert(found,"missing Candelabrum");
