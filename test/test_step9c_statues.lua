-- Exhaustive selection test of the production map, not a second generator.
local map = assert(arg[1])
local face_roll, god_roll, expecting_god
local statues
nh = {rn2=function(n)
  if n==30 then expecting_god=face_roll==25; return face_roll end
  assert(n==3 and expecting_god); expecting_god=false; return god_roll
end}
des = setmetatable({object=function(o) statues[#statues+1]=o end},
                   {__index=function() return function() end end})
local names={
 "Elvenking", "Elvenking", "Elvenqueen", "Elvenqueen",
 "elf-lord", "elf-lord", "elf-lady", "elf-lady",
 "doppelganger", "doppelganger", "nobleman", "nobleman",
 "noblewoman", "noblewoman", "dark young", "coure eladrin",
 "noviere eladrin", "bralani eladrin", "firre eladrin", "shiere eladrin",
 "ghaele eladrin", "tulani eladrin", "dracae eladrin", "Masked Queen",
 "Queen of Stars", "god", "dread seraph", "titan", "titan", "titan"
}
local gods={"the goat with a thousand young", "the misty mother",
            "the wandering dancer"}
for f=0,29 do
 for g=0,(f==25 and 2 or 0) do
  face_roll,god_roll=f,g;statues={}
  dofile(map)
  assert(#statues==31)
  local seen={}
  for _,o in ipairs(statues) do
   assert(o.id=="statue" and o.buc=="blessed" and o.historic)
   assert(o.faceless==(f~=23 and f~=25))
   assert(not o.spe)
   assert((o.name or o.montype)==(f==25 and gods[g+1] or names[f+1]))
   local k=table.concat(o.coord,",");assert(not seen[k]);seen[k]=true
  end
 end
end
print("PASS 31 statues, all 30 weighted faces, 3 gods, faceless/historic and unique coordinates")
