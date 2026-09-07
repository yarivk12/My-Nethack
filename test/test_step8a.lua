-- Read-only level inspection except for test snapshots in the existing Lua
-- variable store. Run on each generated Moria level, then on the return trip.
assert(nh.dnum_name(u.dnum)=="The Ruins of Moria")
local stairs=nh.stairways()
assert(#stairs==(u.dlevel==1 and 1 or 2),"missing/duplicate Moria stairs")
local up,down=0,0
for _,s in ipairs(stairs) do
 if s.up then
  up=up+1;assert(s.dnum==u.dnum and s.dlevel==u.dlevel-1)
 else
  down=down+1
  if u.dlevel==6 then
   assert(s.dnum==0 and s.dlevel>=30 and s.dlevel<=199)
   nh.variable("step8_moria_parent",s.dlevel)
  else assert(s.dnum==u.dnum and s.dlevel==u.dlevel+1) end
 end
end
assert(down==1 and up==(u.dlevel==1 and 0 or 1))
local ox,oy=nh.abscoord(0,0)
local counts,rows={},{}
for y=0,20 do for x=1,79 do
 local m=nh.getmap(x-ox,y-oy)
 counts[m.typ_name]=(counts[m.typ_name] or 0)+1
 rows[#rows+1]=tostring(m.typ)
 if m.typ_name=="muddy swamp" then assert(not m.has_trap,"trap in bog") end
end end
if u.dlevel==2 then
 assert((counts["dead tree"] or 0)>0,"forest has no dead trees")
 assert((counts["tree"] or 0)>0,"forest has no living trees")
end
if u.dlevel==4 then
 local geometry=table.concat(rows,",")
 local before=nh.variable("step8_barren_geometry")
 if before then assert(before~=geometry,"barren floor failed to regenerate") end
 nh.variable("step8_barren_geometry",geometry)
end
nh.pline(string.format("STEP8 LEVEL PASS %d:%d stairs=%d",u.dnum,u.dlevel,#stairs))
