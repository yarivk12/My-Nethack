-- Step 10C-A fixed map resource: lethe-b
-- Pinned donor: neutrality.des at commit 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0

des.level_flags("hardfloor")
des.map({halign="center",valign="center",map=[=[
0                                                                            
1   ...            ###.......  ....            ....                          
2  .....         ###   ...........           .......####                ...  
3  ....      #####    ...}}}}}}}}}}}}}}     .......    ##         ....H....  
4  ...   ##H##        . }}}}}}}}}}}}}}}}     .....      ##       .....  ...  
5   #    #   ###     ..}}}}}}}}}}}}}}}}}}       #        #        ...        
6   #H####     #     .}}}}    ....    }}}}    ###     ....          ####     
7     #        ####..}}}}.     ..      }}}}}..#    ......              #     
8     #             }}}}..      #       }}}I}     ........      .......#     
9     .  ..}I}}}}}}}}}}..   ..###      ...I}}}      .....    .... ....       
0     ....}I}I}}}}}}}}     ...      ##.... }}}}...###       ..}}}}}}}}}}}}}} 
1     .. }}}.}I}}}}}}      .....#      .....}}I}}           .}.}}}}}}}}}}}}} 
2     ..}}}}  ....          ...          .....}}}}          }}}.}}}}}}}}}}}} 
3    ..}}}}      H           #             ... }}}}}}}}}}}}}}}}}.            
4}}}}}}}}}       .....     ###           ##. ...}}}}}}}}}}}}}}}.    ...      
5}}}}}}}}.    ...........  # #  ....     #     . }}}}}}}}}}}}}.    .....     
6}}}}}}}.    .............## ##.....     #     #   .... ......    ......     
7   .....    ...........       ......#####     ###  ......         #         
8            .....  ...         ....             ####  ....####H####         
9                                                                            
]=]})
des.region({region={0,0,75,19},lit=1,type="ordinary"})
des.region({region={2,1,6,4},lit=0,type="ordinary"})
des.region({region={12,14,24,18},lit=0,type="ordinary"})
des.region({region={26,7,30,10},lit=0,type="ordinary"})
des.region({region={30,15,35,18},lit=0,type="ordinary"})
des.region({region={43,1,50,4},lit=0,type="ordinary"})
des.region({region={49,6,56,9},lit=0,type="ordinary"})
des.region({region={65,14,70,16},lit=0,type="ordinary"})
des.region({region={64,3,68,5},lit=0,type="ordinary"})
des.region({region={70,2,73,4},lit=0,type="ordinary"})
des.levregion({region={32,16,33,17},exclude={0,0,0,0},type="branch"})
des.stair({dir="up",coord={4,2}})
des.stair({dir="down",coord={72,3}})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
if percent(75) then
des.object({})
end
if percent(75) then
des.object({})
end
if percent(50) then
des.object({})
end
if percent(25) then
des.object({})
end
des.trap("rust")
des.trap("rust")
des.trap("rust")
des.trap("rust")
des.trap("random")
des.trap("random")
des.monster({class="h",id="deep one"})
des.monster({class="h",id="deep one"})
des.monster({class="h",id="deep one"})
if percent(66) then
des.monster({class="h",id="deep one"})
end
if percent(33) then
des.monster({class="h",id="deep one"})
end
des.monster({class="h",id="deeper one"})
if percent(50) then
des.monster({class="h",id="deeper one"})
end
des.monster({class=";"})
des.monster({class=";"})
des.monster({class=";"})
des.monster({class=";"})
des.monster({class=";"})
if percent(50) then
des.monster({class=";"})
end
des.monster({class=";",id="electric eel"})
des.monster({class=";",id="electric eel"})
if percent(50) then
des.monster({class=";",id="electric eel"})
end
des.monster({})
des.monster({})
des.monster({})
des.monster({})
des.monster({})
des.monster({})
des.monster({class="B"})
des.monster({class="B"})
des.monster({class="h",id="deep one",coord={31,16}})
des.monster({class="h",id="deep one",coord={31,18}})
if percent(50) then
des.monster({class="h",id="deep one",coord={33,18}})
end
des.monster({class="h",id="deep one",coord={28,12}})
des.monster({class="h",id="deep one",coord={22,16}})
des.monster({class="h",id="deep one",coord={20,16}})
des.monster({class="h",id="deep one",coord={20,17}})
if percent(50) then
des.monster({class="h",id="deep one",coord={16,17}})
end
des.monster({class="h",id="deeper one",coord={18,16}})
if percent(50) then
des.monster({class="h",id="deeper one",coord={18,17}})
end
des.object({id="chest",coord={14,18},class="("})
des.monster({class=";",id="electric eel",coord={62,13}})
des.monster({class="T",coord={67,15},asleep=true})
des.monster({class="T",coord={69,16},asleep=true})
des.object({id="chest",coord={68,14},class="("})
des.object({id="water",coord={43,3},class="!",buc="blessed",spe=0})
des.object({id="gain ability",coord={49,8},class="!"})
des.monster({class="p",id="iron piercer",coord={47,3}})
des.monster({class="p",coord={52,8}})
if percent(80) then
des.monster({class="H",id="lurking one",coord={2,2}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={27,1}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={44,2}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={64,4}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={73,3}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={63,8}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={59,10}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={66,15}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={54,15}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={44,14}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={29,6}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={28,11}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={31,18}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={16,18}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={7,17}})
end
if percent(10) then
des.monster({class="'",id="living doll"})
end
if percent(7) then
des.monster({class="A",id="kuker"})
end
if percent(7) then
des.monster({class="A",id="kuker"})
end
if percent(10) then
des.monster({class="X",id="bestial dervish"})
end
if percent(10) then
des.monster({class="X",id="ethereal dervish"})
end
if percent(10) then
des.monster({class="P",id="sparkling lake"})
end
if percent(10) then
des.monster({class="P",id="flashing lake"})
end
if percent(10) then
des.monster({class="P",id="smoldering lake"})
end
if percent(10) then
des.monster({class="P",id="frosted lake"})
end
if percent(10) then
des.monster({class="v",id="blood shower"})
end
if percent(10) then
des.monster({class="U",id="many-taloned thing"})
end
if percent(10) then
des.monster({class="b",id="deep blue cube"})
end
if percent(10) then
des.monster({class="b",id="pitch black cube"})
end
if percent(10) then
des.monster({class="U",id="prayerful thing"})
end
if percent(10) then
des.monster({class="U",id="hemorrhagic thing"})
end
if percent(10) then
des.monster({class="e",id="many-eyed seeker"})
end
if percent(10) then
des.monster({class=" ",id="voice in the dark"})
end
if percent(10) then
des.monster({class="y",id="tiny being of light"})
end
if percent(10) then
des.monster({class="s",id="man-faced millipede"})
end
if percent(10) then
-- Donor class '{' is not a local monster class; identity remains name-resolved.
des.monster({id="mirrored moonflower"})
end
if percent(10) then
des.monster({class="w",id="crimson writher"})
end
if percent(10) then
des.monster({class="p",id="radiant pyramid"})
end
