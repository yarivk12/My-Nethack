-- NetHack 5.0 Lua conversion; see doc/step9c.md.
-- dNetHack 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0:dnethack-3.4.3/dat/chaos2.des
des.level_init({style="solidfill",fg=" "})
des.level_flags("shortsighted","hardfloor")
des.level_init({style="mines",fg="}",bg="Q",smoothed=false,joined=false,lit=true,walled=false})
des.levregion({region={0,0,35,20},exclude={20,0,50,20},type="portal",name="mith1"})
des.map({halign="center",valign="top",map=[=[
QQQQ---------QQQQQ
-----.......-----Q
|.....|-+-|.....|Q
|.--+-|...|---|.|Q
|.|...|...|...|.|Q
|.|...|...|...+.|Q
|.|...----|...|.|Q
|.|---|...|---|.|Q
|.|...|...+.....|Q
|.+...|...|-------
|.|...----|..|...|
|.-----...|..|...|
|.....+...|..|...|
-----.|...|..|...|
QQQQ|.-----+---+--
QQQQ|...........QQ
QQQQ-------------Q
]=]})
des.levregion({region={15,8,15,8},exclude={0,0,0,0},type="branch"})
des.door("closed",4,3)
des.door("closed",2,9)
des.door("closed",10,8)
des.door("closed",8,2)
des.door("closed",6,12)
des.door("closed",14,5)
des.door("closed",11,14)
des.door("closed",15,14)
local shops={"sea garden","fishery","sand-walker shop","spa"}
des.region({region={3,4,5,6},lit=1,type=shops[math.random(1,4)],filled=1})
des.region({region={7,3,9,5},lit=1,type=shops[math.random(1,4)],filled=1})
des.region({region={11,4,13,6},lit=1,type=shops[math.random(1,4)],filled=1})
des.region({region={7,7,9,9},lit=1,type=shops[math.random(1,4)],filled=1})
des.region({region={3,8,5,10},lit=1,type=shops[math.random(1,4)],filled=1})
des.region({region={7,11,9,13},lit=1,type=shops[math.random(1,4)],filled=1})
des.region({region={11,10,12,13},lit=1,type=shops[math.random(1,4)],filled=1})
des.region({region={14,10,16,13},lit=1,type=shops[math.random(1,4)],filled=1})
