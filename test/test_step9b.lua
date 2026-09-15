-- Inspect actual packaged map terrain and connections, allowing donor flips.
assert(nh.dnum_name(u.dnum)=="The Dragon Caves")
assert(u.dlevel>=1 and u.dlevel<=4)
local stairs=nh.stairways();local up,down=0,0
for _,s in ipairs(stairs) do
 if s.up then
  up=up+1
  if u.dlevel==1 then assert(s.dnum~=u.dnum and s.dlevel>=30 and s.dlevel<=199)
  else assert(s.dnum==u.dnum and s.dlevel==u.dlevel-1) end
 else
  down=down+1;assert(s.dnum==u.dnum and s.dlevel==u.dlevel+1)
 end
end
assert(up==1 and down==(u.dlevel==4 and 0 or 1),"wrong caves stairs")
des.map({x=1,y=0,map="x"})
local bog,tree,water=0,0,0
for y=0,20 do for x=1,78 do
 local t=nh.getmap(x,y).mapchr
 if t=="M" then bog=bog+1 end
 if t=="T" or t=="t" then tree=tree+1 end
 if t=="P" or t=="}" then water=water+1 end
end end
assert(bog>0 and tree>0 and water>0,"missing donor terrain / fallback level")
nh.pline(string.format("PASS actual drgn level %d: bog=%d trees=%d water=%d",u.dlevel,bog,tree,water))
