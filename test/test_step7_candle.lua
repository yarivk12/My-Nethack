-- My-NetHack modification notice, added 2026-10-15.
-- Local project integration/modifications; original authorship is retained.
-- Recorded project change dates (Git author dates): 2026-09-07.
-- See doc/public-source-compliance.md and doc/public-source-modifications.json.
-- Existing copyright, attribution, license and warranty notices still apply.
-- #wizloadlua after applying a Magic Candle, and again after save/reload.
local o=u.inventory;
local found=false;
while not o:isnull() do
   local t=o:totable();
   if t.otyp_name=="magic candle" then
      assert(t.lamplit~=0,"Magic Candle must be lit for this check");
      assert(not o:has_timer("burn-obj"),"Magic Candle must not have a fuel timer");
      assert(t.age==300,"donor nominal candle age");
      found=true;
      nh.pline("STEP7 CANDLE PASS lit, age=300, no burn timer; inventory="..t.invlet);
   end
   o=o:next();
end
assert(found,"no carried Magic Candle");
