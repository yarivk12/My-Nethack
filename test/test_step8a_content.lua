-- lua test_step8a_content.lua PINNED_MORIA_DES DAT_DIRECTORY [SEEDS]
-- Independent donor geometry + executable content contracts for all ten maps.
local f=assert(io.open(assert(arg[1]),"rb")); local donor=f:read("*a"); f:close()
donor=donor:gsub("\r\n","\n")
local dat=assert(arg[2]); local seeds=tonumber(arg[3]) or 128
local names={"moria1-1","moria2-1","moria3-1","moria4-1","moria4-2",
             "moria4-3","moria4-4","moria5-1","moria6-1","moria6-2"}
local count=0; for _ in donor:gmatch('\nLEVEL:') do count=count+1 end
assert(count==#names,"donor level/resource count")
local outcomes={forest={},shield=0,fern=0,night=0}
for _,name in ipairs(names) do
    local escaped=name:gsub("%-","%%-")
    local body=assert(donor:match('LEVEL: "'..escaped..'"(.-)\nLEVEL:')
                       or donor:match('LEVEL: "'..escaped..'"(.*)'))
    local geometry=body:match('\nMAP\n(.-)\nENDMAP')
    for seed=1,seeds do
        math.randomseed(seed)
        local trace={objects={},monsters={},stairs={},doors={},traps={},replace={},terrain={}}
        local env=setmetatable({}, {__index=_G})
        env.percent=function(n) return math.random(100)<=n end
        env.d=function(a,b) if not b then return math.random(a) end
            local n=0; for i=1,a do n=n+math.random(b) end; return n end
        env.shuffle=function(t) for i=#t,2,-1 do local j=math.random(i); t[i],t[j]=t[j],t[i] end end
        env.nh={night=function() return seed%2==0 end}
        local selmt={}; selmt.__index=selmt
        function selmt:rndcoord() return {x=self.x1,y=self.y1} end
        selmt.__bor=function(a,b) return a end -- geometry of union independently source-checked
        env.selection={area=function(a,b,c,d) return setmetatable({x1=a,y1=b,x2=c,y2=d},selmt) end,
            circle=function(x,y,r,filled) assert(r==2 and filled==1); return setmetatable({x1=x,y1=y},selmt) end}
        local owner=nil
        env.des={
            level_init=function(t) assert(t.style=="solidfill" and t.fg==" ") end,
            level_flags=function(...) trace.flags={...} end,
            map=function(s) trace.map=s:gsub("\n$","") end,
            region=function(t,light)
                if light then assert(light=="lit" or light=="unlit")
                else assert(t.region and type(t.lit)=="number") end
                if name:match("moria4") then assert(t.lit==-1) end
            end, non_diggable=function() trace.nondig=true end,
            teleport_region=function(t) trace.teleport=t end,
            levregion=function(t) assert(t.type=="branch"); trace.branch=t end,
            stair=function(a,x,y) local t=type(a)=="table" and a or {dir=a,coord={x,y}}
                trace.stairs[#trace.stairs+1]=t end,
            door=function(a,x,y) trace.doors[#trace.doors+1]=type(a)=="table" and a or {state=a,coord={x,y}} end,
            trap=function(a,x,y) trace.traps[#trace.traps+1]={type=a,x=x,y=y} end,
            engraving=function(t) trace.engravings=trace.engravings or {}; table.insert(trace.engravings,t) end,
            grave=function(t) trace.grave=t end,
            altar=function(t) trace.altar=t end,
            message=function(t) trace.message=t end,
            replace_terrain=function(t) table.insert(trace.replace,t) end,
            terrain=function(t) table.insert(trace.terrain,t) end,
            random_corridors=function() trace.corridors=true end,
            room=function(t) trace.rooms=(trace.rooms or 0)+1; if t.contents then t.contents() end end,
        }
        env.des.object=function(t,x,y)
            if not t then t={} elseif type(t)=="string" then
                t={ [#t==1 and "class" or "id"]=t, coord={x,y} }
            end
            t.owner=owner
            table.insert(trace.objects,t)
            if t.contents then local before=owner; owner=t; t.contents(); owner=before end
        end
        env.des.monster=function(t,x,y)
            if type(t)=="string" then t={ [#t==1 and "class" or "id"]=t, coord={x,y} } end
            table.insert(trace.monsters,t)
            if t.inventory then local before=owner; owner=t; t.inventory(); owner=before end
        end
        assert(loadfile(dat.."/"..name..".lua","t",env))()
        assert(trace.map==geometry,name.." geometry differs from pinned donor")
        local n=name:match("moria(%d)")
        local function objects(id,ownercheck)
            local found={}; for _,o in ipairs(trace.objects) do
                if o.id==id and (not ownercheck or o.owner==ownercheck) then table.insert(found,o) end
            end; return found
        end
        local function monsters(id)
            local found={}; for _,m in ipairs(trace.monsters) do if m.id==id then table.insert(found,m) end end
            return found
        end
        if n~="3" then
            local scrolls=objects("scroll of teleportation")
            assert(#scrolls>=1 and #scrolls<=2,name.." recall scroll count")
            for _,o in ipairs(scrolls) do assert(o.buc=="cursed" and o.name=="Word of Recall") end
        end
        assert(#trace.stairs==((n=="1" or n=="6") and 1 or 2),name.." stairs")
        if n=="1" then
            assert(trace.branch.region[1]==43 and trace.branch.region[2]==15)
            assert(#trace.terrain==200 and #trace.objects==200+#objects("scroll of teleportation")+#objects("small piece of unrefined mithril"))
            local ore=objects("small piece of unrefined mithril"); assert(#ore>=2 and #ore<=16)
            for _,o in ipairs(ore) do assert(o.buried and o.coord.x==15 and o.coord.y==12) end
            assert(trace.engravings[1].text=="Herein lie the lower remnants of the Endless Stair.")
        elseif n=="2" then
            assert(#trace.traps==2 and trace.traps[1].x==35 and trace.traps[2].x==36)
            assert(trace.traps[1].type=="hole" and trace.traps[2].type=="hole")
            local boss=monsters("Durin's Bane"); assert(#boss==1 and boss[1].peaceful==false and boss[1].asleep==false)
            local whip=objects("bullwhip",boss[1]); assert(#whip==1 and whip[1].eroded==-1 and whip[1].buc=="uncursed" and whip[1].spe>=1 and whip[1].spe<=7)
            assert(#objects("wand of speed monster",boss[1])==1)
            assert(objects("potion of paralysis",boss[1])[1].buc=="cursed")
            outcomes.shield=outcomes.shield+#objects("shield of reflection",boss[1])
            assert(#objects("dwarvish mithril-coat")>=2 and #objects("dwarvish mithril-coat")<=4)
        elseif n=="3" then
            assert(trace.rooms==7 and trace.corridors and #trace.objects==0 and #trace.monsters==0)
            assert(trace.message=="The dungeon here seems less persistent.")
        elseif n=="4" then
            assert(#monsters("deep orc")>=31 and #monsters("deep orc")<=34)
            assert(#monsters("hill orc")>=21 and #monsters("hill orc")<=24)
            assert(#monsters("orc-captain")>=11 and #monsters("orc-captain")<=14)
            for _,m in ipairs(trace.monsters) do assert(m.peaceful==false) end
            local coats=objects("dwarvish mithril-coat"); assert(#coats>=2 and #coats<=8)
            for _,o in ipairs(coats) do assert(o.buc=="blessed" and o.spe>=1 and o.spe<=5) end
            assert(#trace.doors==(name=="moria4-4" and 18 or 0))
            if name=="moria4-4" then assert(trace.stairs[1].coord[2]==1 and trace.stairs[2].coord[2]==17) end
        elseif n=="5" then
            assert(#monsters("deep orc")==24 and #monsters("dwarf")==12)
            for _,m in ipairs(monsters("dwarf")) do assert(m.peaceful==true) end
            assert(monsters("iron golem")[1].peaceful==true)
            assert(#trace.traps>=2 and #trace.traps<=8)
            assert(trace.replace[1].toterrain=="T" and trace.replace[2].toterrain=="t")
            outcomes.forest[trace.replace[1].chance]=true
            if #trace.replace==3 then assert(trace.replace[3].toterrain=="M"); outcomes.fern=outcomes.fern+1 end
            assert(#monsters("bat")==(seed%2==0 and 2 or 0))
        elseif n=="6" then
            local ruined=name=="moria6-2"
            assert(trace.grave.text=="Balin, Son of Fundin, Lord of Moria")
            assert((trace.altar~=nil)==not ruined)
            local boss=monsters("Watcher in the Water"); assert(#boss==1)
            assert(objects("magic lamp",boss[1])[1].buc=="uncursed")
            assert(#objects("melon")==1 and objects("melon")[1].quantity==1)
            assert(#objects("chest")>=(ruined and 6 or 14))
            assert(#objects("statue")==(ruined and 0 or 4))
            if ruined then assert(#monsters("deep orc")>=12 and #monsters("deep orc")<=20)
            else assert(#objects("iron safe")==2 and #objects("large box")==3 and #objects("ice box")==1) end
        end
    end
    print("PASS "..name..": pinned geometry and "..seeds.." executable content contracts")
end
assert(outcomes.shield>0 and outcomes.shield<seeds)
assert(outcomes.forest[5] and outcomes.forest[20] and outcomes.fern>0 and outcomes.fern<seeds)
print("PASS all ten Moria resources, variants, populations, rewards, terrain choices and night content")
