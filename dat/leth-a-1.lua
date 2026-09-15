-- Step 10C-A fixed map resource: leth-a-1
-- Pinned donor: neutrality.des at commit 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0

des.level_flags("hardfloor")
des.map({halign="center",valign="center",map=[=[
0                                                                            
1   -------                #######H...............#####      ...       ....  
2   |.....|               ##         .......   ...    #####.......    ...... 
3   |..{..+###           ##              #      H     H     .......   .....  
4   |.....|  #############  #########.....      .     ##      ....     ...   
5   ---S---                 #         ......           ###     #        .... 
6      #                ........  }}}}}}}}}}}}}    ....  #     ##     ##.... 
7                    ........}}}}}}}}}}}}}}}}}}}}}}} ....#      #     #  ..  
8             #####H..... .}}}}}}}}}}}}}}}}}}}}}}}}}}}}...    ###     #      
9             #      ...}}}}}}}}}}...........}}}}}}}}}}}}}}   #       ###    
0          ####       .}}}}}}}}}}.....-........}}}}}}}}}}}}}  H         #    
1          #             }}}}}}}}}}}......-...}}}}}}}}}}}}}}}...        #    
2        .....         }}}}}}}}}}}}}}}}}}}}}}}}}}}}  ##  }}}}}.....   .....  
3       .......       }}}}}}}}}}}}}}}}}}}}}}}}}}}...#####..}}}}}}......-.... 
4     ........}}}      }}}}}}}}}}}}}}}}}}}}}}}}....        .}}}}}}}}}}}}}}}}}
5     .........}         ..}}}}}}   }}}}}}} .....           ..}}}}}}}}}}}}}}}
6      .......       ###.....         ........-      ....     ..}}}}}}}}}}}}}
7         .....########     ####         #          ......      ...........  
8                              ###########           .....###H#........      
9                                                                            
]=]})
des.monster({class="&",id="priest of an unknown god",coord={38,10}})
des.altar({coord={35,10},align="noalign",type="altar"})
des.region({region={0,0,75,19},lit=1,type="ordinary"})
des.region({region={4,2,8,4},lit=1,type="ordinary"})
des.region({region={5,12,17,15},lit=0,type="ordinary"})
des.region({region={34,1,48,2},lit=0,type="ordinary"})
des.region({region={58,1,65,4},lit=0,type="ordinary"})
des.region({region={69,1,74,7},lit=0,type="ordinary"})
des.region({region={51,16,56,18},lit=0,type="ordinary"})
des.stair({dir="down",coord={72,2}})
des.door({state="locked",coord={9,3}})
des.door({state="locked",coord={6,5}})
des.monster({class="D",id="red dragon",coord={10,13},asleep=true})
des.monster({class="D",id="red dragon",coord={11,16},asleep=true})
des.object({id="chest",coord={5,14},class="("})
des.object({id="egg",coord={5,15},class="%"})
des.object({id="egg",coord={6,14},class="%"})
des.object({id="egg",coord={6,15},class="%"})
des.object({id="egg",coord={6,16},class="%"})
des.object({id="egg",coord={7,14},class="%"})
des.object({id="egg",coord={7,15},class="%"})
des.object({coord={6,14},class="/"})
des.object({coord={6,14},class="*"})
des.object({coord={6,15},class="*"})
des.object({coord={6,15},class="*"})
des.object({coord={7,13},class="*"})
des.object({id="luckstone",coord={5,15},class="*"})
des.gold({coord={5,14}})
des.gold({coord={5,15}})
des.gold({coord={6,14}})
des.gold({coord={6,15}})
des.gold({coord={6,16}})
des.gold({coord={7,13}})
des.gold({coord={7,14}})
des.gold({coord={7,15}})
des.gold({coord={7,16}})
des.gold({coord={8,15}})
des.gold({coord={8,16}})
des.trap({type="board",coord={10,12}})
des.monster({class=";",coord={63,13}})
des.monster({class=";",coord={28,7}})
des.monster({class=";",coord={22,10}})
des.monster({class=";",coord={59,14}})
des.monster({class=";",coord={39,6}})
des.monster({class=";",coord={26,15}})
des.monster({class=";",id="electric eel",coord={53,8}})
des.monster({class=";",id="electric eel",coord={45,14}})
des.monster({class=";",coord={29,12}})
des.monster({class=";",coord={30,12}})
des.monster({class=";",coord={28,11}})
des.monster({class=";",coord={52,10}})
des.monster({class=";",coord={69,15}})
des.monster({class="g",id="nightgaunt",coord={34,9}})
des.monster({class="g",id="nightgaunt",coord={42,10}})
des.monster({class="g",id="nightgaunt",coord={38,11}})
des.monster({class="g",id="nightgaunt",coord={39,9}})
des.monster({class="g",id="nightgaunt",coord={34,10}})
des.monster({class="g",id="nightgaunt",coord={43,10}})
des.monster({class="g",id="nightgaunt",coord={35,10},asleep=true})
des.monster({class="g",id="nightgaunt",coord={42,9},asleep=true})
if percent(75) then
des.monster({class="g",id="nightgaunt",coord={38,9},asleep=true})
end
if percent(50) then
des.monster({class="g",id="nightgaunt",coord={40,11},asleep=true})
end
des.object({id="statue",coord={33,10},class="`"})
des.object({id="statue",coord={35,9},class="`"})
des.object({id="statue",coord={44,10},class="`"})
des.object({id="statue",coord={36,11},class="`"})
des.object({id="chest",coord={39,10},class="(",contents=function()
if percent(25) then
  des.object({id="magic lamp",class="(",buc="cursed",spe=0})
end
  des.object({id="oil",class="!"})
end})
des.object({id="magic marker",class="("})
if percent(50) then
des.object({id="blank paper",coord={37,10},class="?"})
end
if percent(50) then
des.object({id="blank paper",coord={37,10},class="+"})
end
if percent(50) then
des.object({id="blank paper",coord={41,11},class="?"})
end
if percent(50) then
des.object({id="blank paper",coord={41,11},class="+"})
end
des.monster({class="'",id="living lectern",coord={41,10}})
if percent(25) then
des.monster({class="'",id="living lectern",coord={38,10}})
end
des.monster({class="B",id="vampire bat"})
des.monster({class="B",id="raven"})
des.monster({class="B"})
des.monster({class="B"})
if percent(50) then
des.monster({class="B"})
end
if percent(50) then
des.monster({class="B"})
end
des.trap("rust")
des.trap("rust")
des.trap("rust")
des.trap("pit")
des.trap("pit")
des.trap("random")
des.trap("random")
des.trap("random")
if percent(50) then
des.object({id="leather armor",coord={47,4},class="[",buc="blessed",spe=2})
end
if percent(50) then
des.object({id="leather armor",coord={47,4},class="[",buc="cursed",spe=-2})
end
des.monster({class="'",id="clay golem",coord={4,3},asleep=true})
des.monster({class="'",id="clay golem",coord={7,3},asleep=true})
des.object({id="teleportation",coord={6,6},class="?",buc="cursed",spe=0})
des.object({id="amnesia",coord={6,6},class="?"})
des.monster({class="D",coord={55,18}})
des.gold({coord={51,17}})
des.gold({coord={52,17}})
des.gold({coord={52,16}})
des.gold({coord={53,16}})
des.object({coord={51,17},class="/"})
des.object({coord={51,17},class="*"})
des.object({coord={52,17},class="*"})
des.object({coord={52,16},class="*"})
des.object({coord={53,16},class="*"})
des.object({id="enchant weapon",coord={52,16},class="?"})
des.monster({class="t",id="trapper",coord={62,3}})
des.monster({class="t",id="lurker above",coord={43,1}})
if percent(80) then
des.monster({class="H",id="lurking one",coord={7,13}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={9,17}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={24,15}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={26,2}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={36,2}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={47,4}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={50,6}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={64,2}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={64,2}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={74,2}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={69,12}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={69,18}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={51,17}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={52,13}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={44,16}})
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
