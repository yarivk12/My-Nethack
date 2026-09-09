-- NetHack 5.0 Lua conversion; see doc/step9c.md.
-- dNetHack 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0:dnethack-3.4.3/dat/chaos2.des
des.level_init({style="solidfill",fg=" "})
des.level_flags("shortsighted","hardfloor")
des.map({halign="center",valign="center",map=[=[
 -------------
 |...........|
 |...........|
 |...........|
 |...........|
||...........|
|......-.....|
||...........|
 |...........|
 |...........|
 |...........|
 |...........|
 -------------
]=]})
des.levregion({region={1,6,1,6},exclude={0,0,0,0},type="portal",name="cat1"})
des.object({id="skeleton key",name="The Second Key of Chaos",buc="uncursed",spe=0})
des.monster({id="first wraithworm",coord={7,6}})
des.levregion({region={7,0,12,12},exclude={0,0,0,0},type="portal",name="cat3"})
