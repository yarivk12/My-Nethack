-- Step 10C-A fixed map resource: leth-a-2
-- Pinned donor: neutrality.des at commit 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0

des.level_flags("hardfloor")
des.map({halign="center",valign="center",map=[=[
                                                                            
   -------                #######H...............#####      ...       ....  
   |.....|               ##         .......   ...    #####.......    ...... 
   |...}.+###           ##              #      H     H     .......   .....  
   |.....|  ####H########  #########.....      .     ##      ....     ...   
   ---S---       #         #         ......           ###     #        .... 
      #         ##     ........  }}}}}}}}}}}}}    ....  #     ##     ##.... 
      H         H   ........}}}}}}}}}}}}}}}}}}}}}}} ....#      #     #  ..  
      #      #####H..... .}}}}}}}}}}}}}}}}}}}}}}}}}}}}...    ###     #      
             #      ...}}}}}}}}}}...........}}}}}}}}}}}}}}   #       ###    
          ####       .}}}}}}}}}}.....-........}}}}}}}}}}}}}  H         #    
          #             }}}}}}}}}}}......-...}}}}}}}}}}}}}}}...        #    
        .....         }}}}}}}}}}}}}}}}}}}}}}}}}}}}  ##  }}}}}.....   .....  
       .......   }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}...#####..}}}}}}......-.... 
     ........}}}}}    }}}}}}}}}}}}}}}}}}}}}}}}....        .}}}}}}}}}}}}}}}}}
     .........}         ..}}}}}}   }}}}}}} .....           ..}}}}}}}}}}}}}}}
      .......       ##H.....         ........-      ....     ..}}}}}}}}}}}}}
         .....########     ####         #          ......      ...........  
                              ###########           .....###H#........      
                                                                            
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
des.object({id="statue",coord={33,10},class="`"})
des.object({id="statue",coord={35,9},class="`"})
des.object({id="statue",coord={44,10},class="`"})
des.object({id="statue",coord={36,11},class="`"})
des.object({id="chest",coord={39,10},class="(",contents=function()
if percent(50) then
  des.object({id="magic lamp",class="(",buc="cursed",spe=0})
end
  des.object({id="oil",class="!"})
end})
des.monster({class="B",id="byakhee",coord={9,14},peaceful=false,asleep=true})
des.monster({class="B",id="byakhee",coord={12,15},peaceful=false,asleep=true})
des.monster({class="B",id="byakhee",coord={9,16},peaceful=false,asleep=true})
if percent(60) then
des.monster({class="B",id="byakhee",coord={11,12},peaceful=false,asleep=true})
end
if percent(30) then
des.monster({class="B",id="byakhee",coord={14,14},peaceful=false,asleep=true})
end
des.object({id="luckstone",coord={5,15},class="*"})
des.object({coord={5,14},class="*"})
des.object({coord={7,13},class="*"})
des.object({coord={8,16},class="*"})
des.object({coord={11,13}})
des.object({coord={12,16}})
des.object({coord={6,15}})
des.object({coord={9,13}})
des.trap({type="board",coord={12,10}})
des.trap({type="board",coord={17,17}})
des.monster({class="j",id="ochre jelly",coord={12,16}})
des.monster({class="j",id="ochre jelly",coord={8,12}})
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
des.monster({class="V",id="vampire",coord={62,3},asleep=true,peaceful=false})
des.object({id="speed",coord={62,3},class="!"})
des.monster({class="o",coord={62,4},asleep=true,peaceful=false})
des.monster({class="o",coord={62,2},asleep=true,peaceful=false})
des.monster({class="o",coord={61,3},asleep=true,peaceful=false})
des.monster({class="o",coord={63,3},asleep=true,peaceful=false})
des.trap({type="board",coord={62,5}})
des.trap({type="board",coord={56,2}})
des.object({id="leather armor",coord={47,4},class="[",buc="cursed",spe=-2})
des.monster({class="Z",coord={46,1}})
des.monster({class="Z",coord={44,1}})
des.monster({class="Z",coord={42,1}})
des.monster({class="Z",coord={40,1}})
des.monster({class="Z",coord={38,1}})
des.monster({class="Z",coord={36,1}})
des.monster({class="V",id="vampire lord",coord={5,3},asleep=true,peaceful=false})
des.object({id="create monster",coord={6,6},class="?",buc="cursed",spe=0})
des.object({id="full healing",coord={6,6},class="!"})
des.monster({class="V",id="vampire",coord={6,3},asleep=true,peaceful=false})
des.monster({class="V",id="vampire",coord={4,3},asleep=true,peaceful=false})
des.monster({class="V",id="vampire",coord={5,2},asleep=true,peaceful=false})
des.monster({class="V",id="vampire",coord={5,4},asleep=true,peaceful=false})
des.trap({type="board",coord={10,3}})
des.object({id="teleportation",coord={6,6},class="?",buc="cursed",spe=0})
des.object({id="amnesia",coord={6,6},class="?"})
des.object({id="sickness",coord={6,6},class="!"})
des.object({id="chest",coord={6,8},class="("})
des.gold({coord={6,8}})
des.monster({class="V",id="vampire lord",coord={53,17},asleep=true,peaceful=false})
des.object({id="fire",coord={53,17},class="/"})
des.object({id="slow monster",coord={53,17},class="/"})
des.monster({class="d",id="warg",coord={54,17},asleep=true,peaceful=false})
des.monster({class="d",id="warg",coord={52,18},asleep=true,peaceful=false})
des.monster({class="d",id="winter wolf",coord={53,16},asleep=true,peaceful=false})
des.monster({class="d",id="winter wolf",coord={52,17},asleep=true,peaceful=false})
des.gold({coord={51,17}})
des.gold({coord={52,16}})
des.object({coord={52,17},class="*"})
des.object({coord={53,16},class="*"})
des.trap("rust")
des.trap("magic")
des.trap("magic")
des.trap("pit")
des.trap("pit")
des.trap("random")
des.trap("random")
des.trap("random")
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
