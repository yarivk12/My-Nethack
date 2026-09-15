-- NetHack 5.0 Lua conversion; see doc/step9c.md.
-- dNetHack 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0:dnethack-3.4.3/dat/chaos2.des
des.level_init({style="solidfill",fg=" "})
des.level_flags("nommap","shortsighted")
des.level_init({style="mines",fg=".",bg=" ",smoothed=true,joined=true,lit=false,walled=false})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.monster({id="acid blob"})
des.monster({id="acid blob"})
des.monster({id="acid blob"})
des.monster({id="acid blob"})
des.monster({id="acid blob"})
des.monster({id="acid blob"})
des.monster({id="acid blob"})
des.monster({id="acid blob"})
des.monster({id="acid blob"})
des.monster({id="acid blob"})
des.monster({id="acid blob"})
des.monster({id="acid blob"})
des.monster({id="acid blob"})
des.monster({id="acid blob"})
if percent(50) then des.monster({id="wraithworm"}) end
if percent(50) then des.monster({id="wraithworm"}) end
des.object({})
des.object({})
des.object({})
des.map({halign="center",valign="top",map=[=[
   ..........QQQQQQQQQQQQQQQQQQQQQ.......   
..........QQQQPPPPPPPPQPPPPPPPPPPQQQQ.......
........QQPPPPPPPPPPPPQPPPPPPPPPPPPPPQQ.....
........QQPPPPP|---|PPQPP|-----|PPPPPQQ.....
.......QQQQPP|--...--|Q|--.....--|PPQQQQ....
.......QQQQPP|.......|Q|..  .....|PPQQQQ....
.......QQQQQP|.......|.|.. ......|PQQQQQ....
.......QQQQQP|.......|Q|..  .....|PQQQQQ....
........QQQQP|--...--|Q|--.....--|PQQQQ.....
........QQQQPPP|-+-|PPQPP|---+-|PPPQQQQ.....
..........QQQQPPPPPPPPQPPPPPPPPPPQQQQ.......
 ..........QQQQQQQQQQQQQQQQQQQQQQQQQ....... 
   ..........QQQQQQQQQQQQQQQQQQQQQ.......   
    ............QQQQQQQQQQQQQQQQ........    
]=]})
des.levregion({region={22,6,22,6},exclude={0,0,0,0},type="branch"})
des.door("closed",17,9)
des.object({})
des.object({})
des.object({})
des.map({halign="right",valign="bottom",map=[=[
...
..-
.--
]=]})
des.mazewalk({coord={0,2},dir="west"})
des.stair({dir="down",coord={1,1}})
des.map({halign="left",valign="bottom",map=[=[
...
-..
--.
]=]})
des.mazewalk({coord={2,2},dir="east"})
des.stair({dir="up",coord={1,1}})
