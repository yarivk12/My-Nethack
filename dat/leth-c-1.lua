-- Step 10C-A fixed map resource: leth-c-1
-- Pinned donor: neutrality.des at commit 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0

des.level_flags("hardfloor")
des.map({halign="center",valign="center",map=[=[
0                          }                                                 
1          ..      ........}}|.......                                        
2  ###  ##....   ...........}|..........           ......        .......     
3  H ####  ...  .. .........}|................#+#...........#+##....\..      
4  .    ##...    ..........}}|................#+#.....{.....#+##....\.       
5 ...      ...    ......    }        -.......    .........       .......     
6 ...     ..     ....      }    }}     .....        ....                     
7 ....          ...      -}}} }}}     ##..##                     ###.....    
8  ...##       ...       }}} }..     ###  ####   #    .....    ###  .......  
9      #   .......      }-   ...   ####    ####    .......####    .......  
0      ............    }}   .... ....##    ##......   ....           .....   
1       ... }}}}}I}}}}}}}   ..........     ..........     #                
2        }}I}I}II}}}}}}}}}   .........    .................         ....   
3       }}}II}I}}I}}}}}}}}}    ......   #+..... .............   #### .....   
4}}}}}}}}}}}}     -...  }}}}           ..  .......{..... .....###  ##......  
5}}}}}}}}}    ..    H    }}}}       -....    ........ .......      # .....   
6}}}}}}}}   .....   #     }}}}}}}}}}}}}}}}   .............         #  ....   
7           ......###      }}}}}}}}}}}}}}}}      ...####           H         
8             ...           }}}}}}}}}}}}}}}}  # # # # # #   ....HHHH         
9                                        }}}}                                
]=]})
-- DEFERRED_10C_C: random_monsters()
des.region({region={0,0,75,19},lit=1,type="ordinary"})
des.region({region={9,1,12,3},lit=0,type="ordinary"})
des.region({region={1,4,4,8},lit=0,type="ordinary"})
des.region({region={30,10,36,14},lit=0,type="ordinary"})
des.region({region={51,8,58,10},lit=0,type="ordinary"})
des.region({region={63,2,70,5},lit=1,type="throne",filled=0})
des.region({region={67,7,73,10},lit=1,type="temple"})
des.altar({coord={70,9},align="random",type="shrine"})
des.region({region={68,12,73,16},lit=0,type="ordinary"})
des.region({region={59,17,62,18},lit=0,type="ordinary"})
des.stair({dir="down",coord={59,18}})
des.stair({dir="up",coord={2,6}})
des.drawbridge({coord={27,3},dir="east",state="open"})
des.monster({class="O",id="ogre king",coord={66,3}})
des.monster({class="O",id="ogre king",coord={66,4}})
des.monster({class="O",id="ogre mage",coord={65,4}})
des.monster({class="O",id="ogre",coord={65,2}})
des.monster({class="O",id="ogre",coord={67,2}})
des.monster({class="O",id="ogre",coord={65,4}})
des.monster({class="O",id="ogre",coord={67,4}})
des.monster({class="k",id="kobold lord",coord={65,3}})
des.object({id="chest",coord={70,2},class="("})
des.object({id="chest",coord={70,5},class="("})
des.monster({class="O",id="ogre mage",coord={39,5}})
des.monster({class="O",id="ogre",coord={45,12}})
des.monster({class="O",id="ogre",coord={54,14}})
des.monster({class="O",id="ogre",coord={54,9}})
if percent(50) then
des.monster({class="O",id="ogre",coord={70,14}})
end
if percent(50) then
des.monster({class="O",id="ogre",coord={30,1}})
end
des.monster({class="O",id="ogre",coord={30,4}})
des.monster({class="O",id="ogre",coord={33,2}})
des.monster({class="O",id="ogre",coord={34,2}})
des.object({id="striking",coord={34,2},class="/"})
des.monster({class="O",id="ogre",coord={48,3}})
des.monster({class="O",id="ogre",coord={48,4}})
des.object({id="fire",coord={48,4},class="/"})
des.monster({class="O",id="ogre",coord={56,9}})
des.object({id="sleep",coord={56,9},class="/"})
des.monster({class=";",id="electric eel",coord={25,8}})
des.monster({class="d",coord={39,3}})
des.monster({class="d",coord={43,12}})
des.monster({class="d",coord={53,15}})
if percent(50) then
des.monster({class="d",coord={50,11}})
end
des.monster({class="@",id="wererat",coord={61,18}})
-- DEFERRED_10C_C: random_monsters()
-- DEFERRED_10C_C: random_monsters()
-- DEFERRED_10C_C: random_monsters()
-- DEFERRED_10C_C: random_monsters()
-- DEFERRED_10C_C: random_monsters()
-- DEFERRED_10C_C: random_monsters()
des.object({coord={13,17},class="*"})
des.object({coord={15,16},class="!"})
des.object({coord={14,15},class="?"})
des.object({coord={69,12},class="%"})
des.object({coord={70,12},class="%"})
des.object({coord={71,13},class="%"})
des.object({coord={72,14},class="("})
des.object({coord={70,13},class="%"})
des.object({coord={71,13},class=")"})
des.object({coord={72,13},class="%"})
des.object({coord={71,14},class="%"})
des.object({coord={72,14},class="("})
des.object({coord={70,15},class="%"})
des.object({coord={71,15},class="%"})
des.object({coord={72,15},class="%"})
des.object({coord={69,16},class="("})
des.object({coord={70,16},class="%"})
if percent(50) then
des.object({coord={71,16},class="%"})
end
if percent(50) then
des.object({coord={72,16},class="["})
end
des.monster({class="s",id="cave spider",coord={71,14}})
des.monster({class="s",id="cave spider",coord={72,13}})
des.monster({class="s",id="cave spider",coord={70,16}})
des.monster({class="s",id="cave spider",coord={69,12}})
-- Donor monster feature preserved: "staircase down"
des.monster({class="m",id="giant mimic",coord={73,14},appear_as="ter:staircase down"})
des.trap({type="level teleport",coord={51,18}})
des.trap({type="spiked pit",coord={24,2}})
des.trap({type="spiked pit",coord={22,3}})
des.trap({type="spiked pit",coord={19,2}})
des.monster({class=";",id="electric eel",coord={9,13}})
des.monster({class=";",coord={33,17}})
des.monster({class=";",coord={22,12}})
des.monster({class=";",coord={17,12}})
des.monster({class="O",id="ogre"})
des.monster({class="O",id="ogre"})
des.monster({class="O",id="ogre"})
des.monster({})
des.monster({})
des.monster({})
des.monster({})
des.object({id="earth",coord={10,3},class="?",buc="blessed",spe=0})
des.object({})
des.object({})
des.object({})
if percent(50) then
des.object({})
end
if percent(50) then
des.object({})
end
if percent(50) then
des.object({})
end
if percent(25) then
des.object({})
end
if percent(25) then
des.object({})
end
if percent(25) then
des.object({})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={2,8}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={12,5}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={11,1}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={16,3}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={32,3}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={32,3}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={55,2}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={70,2}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={70,5}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={71,9}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={71,13}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={73,14}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={59,15}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={54,14}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={44,16}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={36,15}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={20,14}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={20,14}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={13,15}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={31,11}})
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
