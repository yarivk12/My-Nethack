-- lua test/test_step7_content.lua PINNED_TOMB_SOURCE LOCAL_TOMB_SOURCE
-- Executes every secret-path combination and every special reward outcome,
-- comparing all map, door, trap, object and monster calls against the donor.
local function encode(v)
   if type(v) ~= "table" then return type(v)=="function" and "callback" or tostring(v); end
   local keys, out = {}, {};
   for k in pairs(v) do keys[#keys+1]=k; end
   table.sort(keys,function(a,b) return tostring(a)<tostring(b); end);
   for _,k in ipairs(keys) do out[#out+1]=tostring(k).."="..encode(v[k]); end
   return "{"..table.concat(out,",").."}";
end
local function run(path, mask, reward)
   local trace, seen, rolls = {}, {}, 0;
   local env = setmetatable({}, {__index=_G});
   env.percent=function(n)
      rolls=rolls+1;
      if rolls<=4 then assert(n==50); return (mask & (1 << (rolls-1)))~=0; end
      assert(n==30); return rolls-4==reward;
   end;
   env.selection={
      new=function()
         local points={};
         return {set=function(_,x,y) points[#points+1]={x,y}; end,
                 rndcoord=function(_,remove) assert(remove~=0); return table.remove(points,1); end};
      end,
      line=function(...) return {line={...}}; end,
      area=function(...) return {area={...}}; end,
   };
   env.des=setmetatable({}, {__index=function(_,name)
      return function(...)
         local args={...};
         if name=="stair" then
            assert(args[1]=="up" and args[2]==1 and args[3]==9);
            return; -- only approved local structural adaptation
         end
         trace[#trace+1]=name..encode(args);
         if name=="object" then
            local id=type(args[1])=="table" and args[1].id or args[1];
            seen[id or "random"]=(seen[id or "random"] or 0)+1;
            if type(args[1])=="table" and args[1].contents then args[1].contents(); end
         elseif name=="monster" then
            seen[args[1]]=(seen[args[1]] or 0)+1;
         elseif name=="map" then
            local width, height;
            height=0;
            for row in args[1]:gmatch("([^\n]+)\n") do
               row=row:gsub("%d", ""); width=width or #row;
               assert(#row==width,"ragged donor map"); height=height+1;
            end
            assert(height==20 and width==44);
         end
      end;
   end});
   assert(loadfile(path,"t",env))();
   assert(seen.shadow==14 and seen.L==1 and seen.Z==19 and seen.M==11);
   assert(seen.chest==8 and seen["wax candle"]==3);
   local prizes={"magic candle","magic marker","wand of death","wand of polymorph"};
   assert(seen[prizes[reward]]==1);
   return table.concat(trace,"\n");
end
for mask=0,15 do
   for reward=1,4 do
      assert(run(arg[1],mask,reward)==run(arg[2],mask,reward),
             "donor/local content differs");
   end
end
print("PASS: 64 donor/local executions: identical classic map, four secret paths, all reward outcomes, traps, doors, caches, 14 Shadows and L/Z/M population");

-- Guard the Step 6 map against long-string concatenation dropping newlines.
if arg[3] then
   local captured;
   local env=setmetatable({}, {__index=_G});
   env.des=setmetatable({}, {__index=function(_,name) return function(s)
      if name=="map" then captured=s; error("map captured"); end
   end end});
   pcall(assert(loadfile(arg[3],"t",env)));
   assert(captured);
   local rows=0;
   for row in captured:gmatch("([^\n]+)\n") do
      assert(#row==56,"Moloch row lost padding/newline"); rows=rows+1;
   end
   assert(rows==9,"Moloch must retain nine map rows");
   print("PASS: evaluated Moloch map is 56x9");
end
