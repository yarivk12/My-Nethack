-- NetHack 5.0 Lua conversion; see doc/step9c.md.
-- dNetHack 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0:dnethack-3.4.3/dat/chaos2.des
des.level_init({style="solidfill",fg=" "})
des.level_flags("shortsighted","hardfloor")
des.map({halign="center",valign="center",map=[=[
   |||||||     
 |||.....|||   
 |.........|   
||..PPPPP..||  
|..PPPPPPP..|  
|..PPPPPPP..|-|
|..PPP.PPP..S.|
|..PPPPPPP..|-|
|..PPPPPPP..|  
||..PPPPP..||  
 |.........|   
 |||.....|||   
   |||||||     
]=]})
des.stair({dir="up",coord={6,6}})
des.monster({id="crystal ooze",coord={5,3}})

-- The donor's +9 is HISTORIC|FACELESS, not enchantment or a count of
-- additional statues. Preserve its 30 equally weighted identities. Bodies
-- absent from this optional branch use the documented native animation
-- equivalents, with their donor identities retained as inscriptions.
local faces = {
  {"Elvenking"}, {"Elvenking"}, {"Elvenqueen"}, {"Elvenqueen"},
  {"elf-lord"}, {"elf-lord"}, {"elf-lady"}, {"elf-lady"},
  {"doppelganger"}, {"doppelganger"},
  {"human", "nobleman", "male"}, {"human", "nobleman", "male"},
  {"human", "noblewoman", "female"}, {"human", "noblewoman", "female"},
  {"umber hulk", "dark young"},
  {"coure eladrin"}, {"noviere eladrin"}, {"bralani eladrin"},
  {"Alabaster elf-elder", "firre eladrin"},
  {"Alabaster elf-elder", "shiere eladrin"},
  {"Alabaster elf-elder", "ghaele eladrin"},
  {"Alabaster elf-elder", "tulani eladrin"},
  {"Alabaster elf-elder", "dracae eladrin"},
  {"Elvenqueen", "Masked Queen", "female", false},
  {"Elvenqueen", "Queen of Stars", "female"},
  {"titan", "god", nil, false},
  {"Angel", "dread seraph"}, {"titan"}, {"titan"}, {"titan"}
}
local function spire_statue(x,y)
  local face=faces[1+nh.rn2(#faces)]
  local o={id="statue",buc="blessed",historic=true,faceless=face[4]~=false,
           montype=face[1],coord={x,y}}
  if face[2]=="god" then
    local gods={"the goat with a thousand young","the misty mother",
                "the wandering dancer"}
    o.name=gods[1+nh.rn2(3)]
  elseif face[2] then o.name=face[2] end
  if face[3] then o[face[3]]=true end
  des.object(o)
end
spire_statue(1,6)
spire_statue(1,5)
spire_statue(1,4)
spire_statue(2,3)
spire_statue(2,2)
spire_statue(3,2)
spire_statue(4,1)
spire_statue(5,1)
spire_statue(6,1)
spire_statue(7,1)
spire_statue(8,1)
spire_statue(9,2)
spire_statue(10,2)
spire_statue(10,3)
spire_statue(11,4)
spire_statue(11,5)
spire_statue(1,7)
spire_statue(1,8)
spire_statue(2,9)
spire_statue(2,10)
spire_statue(3,10)
spire_statue(4,11)
spire_statue(5,11)
spire_statue(6,11)
spire_statue(7,11)
spire_statue(8,11)
spire_statue(9,10)
spire_statue(10,10)
spire_statue(10,9)
spire_statue(11,8)
spire_statue(11,7)
des.levregion({region={13,6,13,6},exclude={0,0,0,0},type="portal",name="cat2"})
