-- Step 10C-A fixed map resource: lethe-e
-- Pinned donor: neutrality.des at commit 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0

des.level_flags("hardfloor")
des.map({halign="center",valign="center",map=[=[
0                            ###         ------- }}}}                        
1           ...         -----###-----   --.....--}}}}   .........            
2          ....         |...|###|...|   |.......|}}}} .............          
3       ##.....###      |....###....|   |.......|}}}} ..............####     
4       #  ....  #      |....###....|   |......}}}}}}  ............    # ... 
5  ...  #   ...  #      |...|###|...|   |.....}}|}}}}    ........      ##... 
6 .....##        #      -----###-----   |....}}}|}}}.        #           ..  
7  ...         ###         --###--      -F-}}}-+-}}...      ##  ...          
8              #           -.###.-     }}}-}}}}}}}.- #     ##   ....###....  
9            ......######....###...   }}}}}}.}}}     ##    #    ...  #  ...  
0           .......      ..  ###  .. }}}}}}-}-..      #### #         #  ...  
1          .........    ...  ###  ..}}}}}}}...#      ##  ######     ...      
2         .......-}}}}}}}}}}}###}}}}}}}}}}    ###   ##        ###  ......    
3}}}}}}}}}}}.-...}}}}}}}}}}}}###}}}}}}}}}       #  ##   .....   ##.........  
4}}}}}}}}}}}}}}}}}}}}}}}}}}}}###}}}}}}}}      ######   .......     ........  
5}}}}}}}}}}}}}}}}}}}}}}....  ###  -...      ###           H        ......    
6       }}}}}}}}}}}}}    ....###....     ####       ........     #H....      
8                       .....###.....#####  ##   ##..........H####           
9                        ....###....         #####  ........                 
0                            ###                                             
]=]})
des.region({region={39,0,47,7},lit=1,type="temple"})
des.altar({coord={43,2},align="neutral",type="shrine"})
des.region({region={0,0,51,19},lit=1,type="ordinary"})
des.region({region={1,5,5,7},lit=0,type="ordinary"})
des.region({region={9,1,13,5},lit=0,type="ordinary"})
des.region({region={24,2,26,5},lit=0,type="ordinary"})
des.region({region={31,2,34,5},lit=0,type="ordinary"})
des.region({region={50,16,59,18},lit=0,type="ordinary"})
des.region({region={54,13,60,14},lit=0,type="ordinary"})
des.region({region={60,14,73,16},lit=1,type="ordinary"})
des.region({region={53,1,66,5},lit=1,type="ordinary"})
des.region({region={72,4,74,6},lit=0,type="ordinary"})
des.region({region={63,7,66,9},lit=0,type="ordinary"})
des.region({region={70,8,73,10},lit=0,type="ordinary"})
des.stair({dir="down",coord={73,4}})
des.stair({dir="up",coord={3,7}})
des.monster({class="T",id="troll",coord={24,2},asleep=true})
des.monster({class="T",id="troll",coord={26,2},asleep=true})
des.monster({class="T",id="troll",coord={24,5},asleep=true})
des.monster({class="T",id="troll",coord={26,5},asleep=true})
des.monster({class="T",id="troll",coord={32,2},asleep=true})
des.monster({class="T",id="troll",coord={34,2},asleep=true})
des.monster({class="T",id="troll",coord={32,5},asleep=true})
des.monster({class="T",id="troll",coord={34,5},asleep=true})
des.monster({class="T",asleep=true})
des.monster({class="T",asleep=true})
des.monster({class="T",asleep=true})
des.monster({class="T",asleep=true})
des.monster({class="T",asleep=true})
des.monster({class="T",asleep=true})
des.monster({class="T",asleep=true})
des.monster({class="T",asleep=true})
des.monster({class="T",asleep=true})
des.monster({class="T",asleep=true})
des.monster({class="T",asleep=true})
if percent(50) then
des.monster({class="T",asleep=true})
end
if percent(50) then
des.monster({class="T",asleep=true})
end
if percent(50) then
des.monster({class="T",asleep=true})
end
des.monster({class=";",coord={4,14}})
des.monster({class=";",coord={16,15}})
des.monster({class=";",coord={25,13}})
des.monster({class=";",coord={35,13}})
des.monster({class=";",coord={41,9}})
des.monster({class=";",coord={48,6}})
if percent(50) then
des.monster({class=";",coord={50,1}})
end
des.monster({})
des.monster({})
des.monster({})
des.monster({})
des.monster({})
if percent(50) then
des.monster({})
end
if percent(50) then
des.monster({})
end
if percent(50) then
des.monster({})
end
des.object({id="full healing",class="!"})
des.object({id="full healing",class="!"})
des.object({id="extra healing",class="!"})
des.object({id="diamond",class="*"})
des.object({id="diamond",class="*"})
des.object({id="diamond",class="*"})
des.object({class="%"})
des.object({class="%"})
des.object({class="%"})
des.object({class="%"})
des.object({class="%"})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.trap("random")
des.trap("random")
des.trap("random")
des.trap("random")
des.trap("random")
des.trap("random")
if percent(80) then
des.monster({class="H",id="lurking one",coord={1,6}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={10,2}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={11,13}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={13,13}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={23,11}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={33,11}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={49,8}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={55,1}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={73,5}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={64,8}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={72,9}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={69,14}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={57,13}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={34,15}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={21,15}})
end
if percent(7) then
des.monster({class="A",id="kuker"})
end
if percent(7) then
des.monster({class="A",id="kuker"})
end
