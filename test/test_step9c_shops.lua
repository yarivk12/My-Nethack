-- Validate the actual Elshava resource's eight independently selected shops.
-- Run from the repository root with the standalone audit Lua interpreter.
local counts, total = {}, 0
local original_random = math.random
math.random = function(lo, hi)
  assert(lo == 1 and hi == 4)
  total = total + 1
  return 1 + ((total - 1) % 4)
end
des = {
  level_init=function() end,
  level_flags=function() end,
  levregion=function() end,
  map=function() end,
  door=function() end,
  region=function(t)
    assert(t.filled == 1, "Elshava merchant region must be explicitly filled")
    assert(t.lit == 1, "region lighting must use native integer form")
    counts[t.type] = (counts[t.type] or 0) + 1
  end
}
dofile("dat/ossa1.lua")
math.random = original_random
assert(total == 8)
for _, name in ipairs({"sea garden","fishery","sand-walker shop","spa"}) do
  assert(counts[name] == 2, "missing independently selected shop: "..name)
end
print("PASS eight independently selected, lit and filled Elshava merchant regions")
