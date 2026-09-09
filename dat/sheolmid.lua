-- NetHack special level; see doc/step9a.md.
-- UnNetHack 439b8d63d3d1ca78fb08588dd43f61874114b21a:dat/sheol.des
des.level_init({style="solidfill", fg=" "})
des.level_flags("hardfloor", "noteleport", "noflip")
des.level_init({style="sheol"})
-- Donor GEOMETRY:(0,2) is relative to its full-level origin (1,0).
des.map({x=1, y=2, map=[=[
xxxxxPxxIPxxxPPxxxxxxxxxxx
xPUPPPPPIPPPPPPxPPPPPPxxxx
xPPPPPPUIPPPPPPPPUIPPPP.xx
YPPPPPPPPPPPPPPUPPIIPPPP.x
Y...........PPPPPPPPPIPP..
Y............PPPPPPPPUIP..
Y.--------....PPPPPPPPPPP.
Y.|......|....PPPPPPPPPPP.
Y.|......+....PPPPPPPPPPP.
Y.|......|....PPPPPPPPIPP.
Y.--------....PPPPPPPPPPP.
Y............PPPPUIPIPPP..
Y...........PPPPPPIIPPPP..
YPPPPPPPPPPPPPPPPPPPPPPP.x
xPPPUPPIPPPPPPPPPPPUPPP.xx
xxPxPPPPPPPPPUIPPIIPPPxxxx
xxPxxxPxxIPPIIPxxUPxxxxxxx
]=]})
local guards = {"X","T","L","'","N","x","@","V"}
shuffle(guards)
des.stair("down",4,8)
des.levregion({region={65, 0, 79, 20}, exclude={1, 0, 64, 20}, type="stair-up", region_islev=1, exclude_islev=1})
-- Donor Valley shortcut omitted: optional DoD side branch.
des.door("locked",9,8)
des.object({class="/",id="fire"})
des.object({class="/",id="fire"})
des.monster({id="crystal ice golem"})
des.monster({id="blue slime"})
des.monster({class=guards[1],coord={3, 7}})
des.monster({class=guards[1],coord={3, 9}})
des.monster({class=guards[2],coord={4, 7}})
des.monster({class=guards[2],coord={4, 9}})
des.monster({class=guards[3],coord={5, 7}})
des.monster({class=guards[3],coord={5, 9}})
des.monster({class=guards[4],coord={6, 7}})
des.monster({class=guards[4],coord={6, 9}})
des.monster({class=guards[5],coord={7, 7}})
des.monster({class=guards[5],coord={7, 9}})
des.monster({class=guards[6],coord={8, 7}})
des.monster({class=guards[6],coord={8, 9}})
des.monster({class=guards[7],coord={10, 7}})
des.monster({class=guards[7],coord={10, 9}})
des.monster({class=guards[8],coord={1, 7}})
des.monster({class=guards[8],coord={1, 9}})
des.map({x=1,y=0,map="x"}) -- reset the full coordinate frame
des.region({region={0,0,30,19},lit=1,type="ordinary"})
des.object({class="*"})
des.object({class="!"})
des.object({class="\""})
des.object({class="?"})
des.object({class="/"})
des.object({class="="})
des.object({class="+"})
des.object({class="("})
des.object({class="("})
des.object({class=")"})
des.object({class=")"})
des.trap()
des.trap()
des.trap()
des.trap()
des.trap()
des.trap()
des.monster()
des.monster()
des.monster()
des.monster()
des.monster()
des.monster()
des.monster()
des.monster()
