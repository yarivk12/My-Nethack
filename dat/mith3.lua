-- NetHack 5.0 Lua conversion; see doc/step9c.md.
-- dNetHack 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0:dnethack-3.4.3/dat/chaos2.des
des.level_init({style="solidfill",fg=" "})
des.level_flags("shortsighted","hardfloor")
des.level_init({style="mines",fg="-",bg="s",smoothed=false,joined=false,lit=true,walled=false})
des.levregion({region={0,0,75,20},exclude={25,0,50,20},type="portal",name="mith2"})
des.object({id="ceramic tile"})
des.object({id="ceramic tile"})
des.object({id="ceramic tile"})
des.object({id="ceramic tile"})
des.object({id="ceramic tile"})
des.object({id="ceramic tile"})
if percent(75) then des.object({id="ceramic tile"}) end
if percent(75) then des.object({id="ceramic tile"}) end
if percent(50) then des.object({id="ceramic tile"}) end
if percent(50) then des.object({id="ceramic tile"}) end
if percent(25) then des.object({id="ceramic tile"}) end
if percent(25) then des.object({id="ceramic tile"}) end
des.monster({id="Alabaster elf"})
des.monster({id="Alabaster elf"})
des.monster({id="Alabaster elf"})
des.monster({id="Alabaster elf"})
des.monster({id="Alabaster elf"})
des.monster({id="Alabaster elf"})
if percent(50) then des.monster({id="wraithworm"}) end
if percent(50) then des.monster({id="wraithworm"}) end
des.map({halign="center",valign="center",map=[=[
sssssssssssssssssss
ssssseeeeeeeeesssss
ss||eeeeeeeeeee||ss
ss|eeeeeeeeeeeee|ss
sseeeeeeeeeeeeeeess
seeeee|||||||eeeees
|eeee||.....||eeee|
|eeee|.......|eeee|
|eeee|.......|eeee|
|eeee|.......|eeee|
|eeee|.......|eeee|
|eeee|.......|eeee|
|eeee||.....||eeee|
seeeee|||||||eeeees
sseeeeeeeeeeeeeeess
ss|eeeeeeeeeeeee|ss
ss||eeeeeeeeeee||ss
ssssseeeeeeeeesssss
sssssssssssssssssss
sssssssssssssssssss
]=]})
if percent(50) then des.monster({id="wraithworm"}) end
if percent(50) then des.monster({id="wraithworm"}) end
des.stair({dir="down",coord={9,9}})
