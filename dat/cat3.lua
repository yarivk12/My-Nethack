-- NetHack 5.0 Lua conversion; see doc/step9c.md.
-- dNetHack 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0:dnethack-3.4.3/dat/chaos2.des
des.level_init({style="solidfill",fg=" "})
des.level_flags("shortsighted","hardfloor")
des.map({halign="center",valign="center",map=[=[
     |||||||     
   |||.-.-.|||   
   |-.-...-.-|   
  ||.-.....-.||  
  |.-.......-.|  
--|-.........-|--
|.S...........S.|
--|-.........-|--
  |.-.......-.|  
  ||.-.....-.||  
   |-.-...-.-|   
   |||.-.-.|||   
     |||||||     
]=]})
local places = {{3,4},{5,2},{8,1},{3,8},{5,10},{8,11},{13,4},{11,2},{13,8},{11,10}}
shuffle(places)
des.levregion({region={1,6,1,6},exclude={0,0,0,0},type="portal",name="mith3"})
des.stair({dir="down",coord={15,6}})
des.object({id="robe",coord={8,5}})
des.object({id="elven boots",coord={8,5}})
des.object({id="elven toga",coord={8,5}})
des.object({id="leather gloves",coord={8,5}})
des.object({id="quarterstaff",material="metal",anarchic=true,coord={8,5}})
des.object({id="mask",material="mineral",montype="human",eroded=1,coord={8,5}})
des.object({id="robe",coord={8,7}})
des.object({id="elven boots",coord={8,7}})
des.object({id="elven toga",coord={8,7}})
des.object({id="leather gloves",coord={8,7}})
des.object({id="quarterstaff",material="metal",anarchic=true,coord={8,7}})
des.object({id="mask",material="mineral",montype="human",eroded=1,coord={8,7}})
des.object({id="slab",coord=places[1]})
des.object({id="ceramic tile",coord=places[2]})
des.object({id="ceramic tile",coord=places[3]})
des.object({id="ceramic tile",coord=places[4]})
if percent(66) then des.object({id="ceramic tile",coord=places[5]}) end
if percent(66) then des.object({id="ceramic tile",coord=places[6]}) end
if percent(66) then des.object({id="ceramic tile",coord=places[7]}) end
if percent(33) then des.object({id="ceramic tile",coord=places[8]}) end
if percent(33) then des.object({id="ceramic tile",coord=places[9]}) end
if percent(33) then des.object({id="ceramic tile",coord=places[10]}) end
