-- Compare executable Lua content with the pinned Sheol .des, 256 seeds/map.
local f=assert(io.open(assert(arg[1]),"rb"))
local donor=f:read("*a"):gsub("\r\n","\n"); f:close()
local dat=assert(arg[2])
local names={"sheolfil","sheolmid","palace_f","palace_e"}
local chances={}
local function count(s,p) local n=0;for _ in s:gmatch(p) do n=n+1 end;return n end
for _,name in ipairs(names) do
 local body=assert(donor:match('LEVEL: "'..name..'"(.-)\nLEVEL:')
                or donor:match('LEVEL: "'..name..'"(.*)'))
 body=body:gsub("#[^\n]*","")
 local geometry=body:match("\nMAP\n(.-)\nENDMAP")
 local expected={monsters={},objects={}}
 local conditional={monsters=0,objects=0}
 for line in body:gmatch("[^\n]+") do
  local kind=line:match("MONSTER:") and "monsters"
          or (line:match("OBJECT:") or line:match("CONTAINER:")) and "objects"
  if kind then
   local id=line:match(',%s*"([^"]+)"%s*%)')
   local key=id or "random"
   if line:match("%[%d+%%%]") then
    conditional[kind]=conditional[kind]+1
   else
    expected[kind][key]=(expected[kind][key] or 0)+1
   end
  end
 end
 local trap_count=count(body,"TRAP:")
 for seed=1,256 do
  math.randomseed(seed)
  local trace={monsters={},objects={},maps={},stairs={},regions={},doors={},graves={}}
  local env=setmetatable({}, {__index=_G})
  env.percent=function(n)
   assert((name=="sheolfil" and (n==50 or n==33))
          or (name=="palace_e" and (n==10 or n==30)))
   local hit=math.random(100)<=n
   local key=name..":"..n;chances[key]=(chances[key] or 0)+(hit and 1 or 0)
   return hit
  end
  env.shuffle=function(t) for i=#t,2,-1 do local j=math.random(i);t[i],t[j]=t[j],t[i] end end
  env.selection={area=function(...) return {...} end}
  local trap=0
  env.des={
   level_init=function(t) assert(t.style=="solidfill" or t.style=="sheol");
    if t.style=="sheol" then trace.generator=true end end,
   level_flags=function(...) trace.flags={...} end,
   map=function(t) trace.maps[#trace.maps+1]=t end,
   levregion=function(t) assert(t.type=="stair-up" or t.type=="stair-down");trace.stairs[#trace.stairs+1]=t end,
   stair=function(dir,x,y) trace.stairs[#trace.stairs+1]={dir=dir,x=x,y=y} end,
   region=function(t) trace.regions[#trace.regions+1]=t end,
   teleport_region=function(t) trace.teleport=t end,
   non_diggable=function(t) trace.nondig=t end,
   door=function(state,x,y) trace.doors[#trace.doors+1]={state,x,y} end,
   trap=function(t) assert(t==nil);trap=trap+1 end,
   grave=function(t) trace.graves[#trace.graves+1]=t.text end,
  }
  for _,kind in ipairs({"monsters","objects"}) do
   env.des[kind=="monsters" and "monster" or "object"]=function(t)
    t=t or {}; trace[kind][#trace[kind]+1]=t
    if t.contents then t.contents() end
   end
  end
  assert(loadfile(dat.."/"..name..".lua","t",env))()
  assert(#trace.flags==3 and table.concat(trace.flags,",")=="hardfloor,noteleport,noflip")
  assert(trap==trap_count,name.." traps")
  assert(#trace.stairs==count(body,"STAIR:"),name.." stairs")
  assert(#trace.doors==count(body,"DOOR:"),name.." doors")
  assert(trace.generator==(name=="sheolfil" or name=="sheolmid") or
         (not trace.generator and name:match("palace")))
  if geometry then
   local found=0
   for _,m in ipairs(trace.maps) do
    if m.map~="x" then
     assert(m.map:gsub("\n$","")==geometry,name.." exact donor geometry")
     if name=="sheolmid" then assert(m.x==1 and m.y==2)
     else assert(m.halign=="center" and m.valign=="center") end
     found=found+1
    end
   end
   assert(found==1)
  end
  for _,kind in ipairs({"monsters","objects"}) do
   local actual={};for _,t in ipairs(trace[kind]) do
    local key=t.id or "random";actual[key]=(actual[key] or 0)+1
   end
   local extra=0
   for key,n in pairs(expected[kind]) do assert((actual[key] or 0)>=n,name.." missing "..key) end
   for key,n in pairs(actual) do extra=extra+n-(expected[kind][key] or 0) end
   assert(extra>=0 and extra<=conditional[kind],name.." unexpected "..kind)
  end
  if name=="sheolmid" then
   local guards={};for _,m in ipairs(trace.monsters) do
    if m.coord then guards[m.class]=(guards[m.class] or 0)+1 end
   end
   local classes=0;for _,n in pairs(guards) do assert(n==2);classes=classes+1 end
   assert(classes==8 and trace.regions[1].lit==1)
  elseif name=="palace_e" then
   local chests=0;local picks=0;local marker=0
   for _,o in ipairs(trace.objects) do
    if o.id=="chest" then chests=chests+1 end
    if o.id=="crystal pick" then picks=picks+1 end
    if o.id=="magic marker" then marker=marker+1 end
   end
   assert(chests==7 and picks==2 and marker==1 and trace.nondig)
  end
 end
 print("PASS "..name..": 256 seeded donor geometry/content/flags/connection comparisons")
end
for key,n in pairs(chances) do
 local p=tonumber(key:match(":(%d+)$"))/100
 assert(math.abs(n/256-p)<.12,key.." probability")
 print("PASS conditional "..key.." "..n.."/256")
end
