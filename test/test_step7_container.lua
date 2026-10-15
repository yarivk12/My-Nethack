-- My-NetHack modification notice, added 2026-10-15.
-- Local project integration/modifications; original authorship is retained.
-- Recorded project change dates (Git author dates): 2026-09-07.
-- See doc/public-source-compliance.md and doc/public-source-modifications.json.
-- Existing copyright, attribution, license and warranty notices still apply.
-- #wizloadlua after putting a lit Magic Candle into a carried sack.
local o, found = u.inventory, false;
while not o:isnull() do
   if o:totable().otyp_name == "sack" then
      local c = o:contents();
      while not c:isnull() do
         local t = c:totable();
         if t.otyp_name == "magic candle" then
            assert(t.lamplit == 0, "contained candle must be snuffed");
            assert(not c:has_timer("burn-obj"), "no candle fuel timer");
            found = true;
         end
         c = c:next();
      end
   end
   o = o:next();
end
assert(found, "no Magic Candle in a carried sack");
nh.pline("STEP7 CONTAINER PASS candle snuffed, no timer");
