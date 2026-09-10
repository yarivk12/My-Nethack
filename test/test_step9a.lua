-- Read-only inspection of an actual packaged Sheol floor.
assert(nh.dnum_name(u.dnum)=="Sheol")
local counts={}
local ox,oy=nh.abscoord(0,0)
for x=1,79 do for y=0,20 do
 local m=nh.getmap(x-ox,y-oy)
 counts[m.mapchr]=(counts[m.mapchr] or 0)+1
end end
assert((counts.Y or 0)>0,"missing crystal ice: possible fallback maze")
if u.dlevel==1 then
 assert((counts.U or 0)>0 and (counts.I or 0)>0,
        "Sheol filler generator not present")
end
local stairs=nh.stairways()
assert(#stairs>=1 and #stairs<=2)
if u.dlevel==1 then
 local returns=0
 for _,s in ipairs(stairs) do
  if s.dnum~=u.dnum then
   assert(s.up and s.dnum==0 and s.dlevel>=30 and s.dlevel<=199)
   returns=returns+1
  end
 end
 assert(returns==1)
end
nh.pline(string.format("SHEOL_CONTENT %d crystal=%d icewall=%d",
                       u.dlevel,counts.Y or 0,counts.U or 0))
