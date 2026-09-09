-- Executable map conversion checks against the pinned Dragon Caves resource.
local f=assert(io.open(assert(arg[1]),"rb"))
local donor=f:read("*a"):gsub("\r\n","\n");f:close()
local dat=assert(arg[2]);local chances={}
for index,name in ipairs({"drgnA","drgnB","drgnC","drgnD"}) do
 local body=assert(donor:match('LEVEL: "'..name..'"(.-)\nLEVEL:')
                   or donor:match('LEVEL: "'..name..'"(.*)'))
 local geometry=assert(body:match("\nMAP\n(.-)\nENDMAP"))
 for seed=1,256 do
  math.randomseed(seed)
  local trace={monsters={},objects={},stairs={},gold={},flags={},maps={},dice={}}
  local env=setmetatable({}, {__index=_G})
  env.percent=function(p)
   assert(index==4 and (p==40 or p==60 or p==80))
   local hit=math.random(100)<=p;chances[p]=chances[p] or {0,0}
   chances[p][1]=chances[p][1]+(hit and 1 or 0);chances[p][2]=chances[p][2]+1
   return hit
  end
  env.d=function(n,sides)
   local value=0;for i=1,n do value=value+math.random(sides) end
   trace.dice[#trace.dice+1]={n,sides,value};return value
  end
  env.selection={area=function(...) return {...} end}
  local traps=0
  env.des={
   level_init=function(t) assert(t.style=="solidfill" and t.fg==" ") end,
   level_flags=function(...) trace.flags={...} end,
   map=function(t) trace.maps[#trace.maps+1]=t end,
   monster=function(t) trace.monsters[#trace.monsters+1]=t end,
   object=function(t) trace.objects[#trace.objects+1]=t or {} end,
   gold=function(t) trace.gold[#trace.gold+1]=t end,
   stair=function(dir,x,y) trace.stairs[#trace.stairs+1]={dir=dir,x=x,y=y} end,
   levregion=function(t) trace.stairs[#trace.stairs+1]=t end,
   non_diggable=function(t) trace.nondig=t end,
   region=function(t) trace.room=t end,
   door=function(state,x,y) trace.door={state,x,y} end,
   trap=function() traps=traps+1 end,
  }
  assert(loadfile(dat.."/"..name..".lua","t",env))()
  assert(#trace.maps==1 and trace.maps[1].map:gsub("\n$","")==geometry,name.." geometry")
  assert(trace.maps[1].halign=="center" and trace.maps[1].valign=="center")
  assert(table.concat(trace.flags,",")==(index==4 and "noteleport,hardfloor" or ""))
  -- No noflip override: retain donor's independent horizontal/vertical flips.
  assert(traps==4 and #trace.stairs==(index==4 and 1 or 2))
  local dragons,worms,chromatic=0,0,{}
  for _,m in ipairs(trace.monsters) do
   assert(m.peaceful==false)
   if m.class=="D" then dragons=dragons+1
   elseif m.class=="w" then worms=worms+1
   else
    assert(m.id=="chromatic cave dragon" and index==4)
    chromatic[#chromatic+1]=table.concat(m.coord,",")
   end
  end
  assert(dragons==({16,17,17,22})[index] and worms==(index==4 and 6 or 4))
  if index==4 then
   assert(table.concat(chromatic,";")=="1,7;2,6;1,6")
   assert(trace.stairs[1].dir=="up" and trace.stairs[1].x==73 and trace.stairs[1].y==18)
   local random_gold=0
   for i,g in ipairs(trace.gold) do
    if i<=10 then
     local ranges={{612,1800},{610,1600},{610,1600},{610,1600},{610,1600},
                   {410,1400},{205,700},{205,700},{205,700},{105,600}}
     assert(g.coord and g.amount>=ranges[i][1] and g.amount<=ranges[i][2])
    else assert(not g.coord and g.amount>=1 and g.amount<=100);random_gold=random_gold+1 end
   end
   assert(random_gold>=2 and random_gold<=20)
   local loopdice=trace.dice[11]
   assert(loopdice[1]==2 and loopdice[2]==10 and loopdice[3]==random_gold)
   assert(#trace.objects>=35 and #trace.objects<=47)
   local boulder,gem=0,0
   for _,o in ipairs(trace.objects) do
    if o.id=="boulder" then boulder=boulder+1;assert(table.concat(o.coord,",")=="1,5") end
    if o.class=="*" then gem=gem+1 end
   end
   assert(boulder==1 and gem==17)
  else
   assert(#chromatic==0 and #trace.gold==0 and #trace.objects==7)
   if index==2 then
    assert(trace.room.type=="ordinary" and trace.room.lit==1)
    assert(table.concat(trace.room.region,",")=="67,9,72,11")
    assert(table.concat(trace.nondig,",")=="64,6,75,15")
    assert(table.concat(trace.door,",")=="open,66,10")
    for _,s in ipairs(trace.stairs) do assert(table.concat(s.region,",")=="0,0,63,20") end
   end
  end
 end
 print("PASS "..name..": 256 seeds, pinned geometry, populations, loot, stairs, flags and terrain")
end
for p,result in pairs(chances) do
 assert(math.abs(result[1]/result[2]-p/100)<.06)
 print("PASS independent "..p.."% hoard rolls: "..result[1].."/"..result[2])
end
