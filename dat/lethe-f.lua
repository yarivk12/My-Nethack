-- Step 10C-A fixed map resource: lethe-f
-- Pinned donor: neutrality.des at commit 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0

des.level_flags("hardfloor")
des.map({halign="center",valign="center",map=[=[
0 ---------                                                            ------
1 |.S.....|    .........    ######                        ......-......|....|
2 |.|.....|..............H###    #######      #####   ....... ..... ...+....|
3 -----+---...... .......              ###.....   ##..... .............------
4    ..................}}}}}}}}}}}}}}}    ..}..     .. ........ .... ....    
5  .................}}}}}}}}}}}}}}}}}}}    ...       ............ ....}}}}}}}
6 ..... ..... ....}}}}}}}}}}}}}}}}}}}}}}     ###      .... .........}}}}}}}}}
7 .... .........}}}}}}}}  ...... }}}}}}}}      #       ....... ...}}}}}}}}}}}
8  ............}}}}}}}}    ..  #  }}}}}}}}}    ##...    ... ....}}}}}}}}}}}} 
9   ....... ..}}}}}}}}         ##  }}}}}}}}}}    ....    .....}}}}}}}}}}     
0     .. .....}}}}}}}}}.        ###.}}}..}}}}..   ....     ..}}}}}}}}        
1       .......}}}}}}}}}.           .}}}..}}}}}.. # ...    }}}}}}    ------- 
2       #   .... .}}}}}}..           .}}. .}}}}}...       }}}}}      |.....| 
3      ##   # ......}}}}}.            ..}}..}}}.}} ..    }}}}}  --------S--- 
4     ##    #        }}}}..       ###  }}.}}}}..}}}}}.} }}}}}   |}}w{wwww|   
5     #   #H##  ....  }}}}.#####  H ##  .}.}}}.}.}}}}}.}}}}}    |}w{.{ww}|   
6    ....H#  ##.....  }}}}.    .. .  ###..}.}.}}}.}}}}}.}}..####+www{ww}}|   
7  .....       ....   }}}}   ......      ..}. -}}}.}}}}}..      ----------   
8   .....       ...  }}}}    .....                                           
9                   }}}}                                                     
]=]})
des.monster({class="q",id="blessed",coord={6,2}})
if percent(20) then
des.monster({class="q",id="blessed",coord={6,7}})
end
if percent(10) then
des.monster({class="q",id="blessed",coord={23,2}})
end
if percent(10) then
des.monster({class="q",id="blessed",coord={11,12}})
end
des.monster({class="q",id="dark young",coord={13,3}})
des.monster({class="q",id="dark young",coord={9,6}})
des.monster({class="q",id="dark young",coord={6,9}})
des.monster({class="q",id="dark young",coord={13,12}})
if percent(66) then
des.monster({class="q",id="dark young",coord={22,3}})
end
if percent(33) then
des.monster({class="q",id="dark young",coord={2,7}})
end
des.monster({class="n",id="deminymph",coord={16,16}})
des.object({id="chest",coord={18,15},class="("})
des.monster({class="n",id="oread",coord={4,17}})
des.monster({class="n",id="oread",coord={5,18}})
des.object({id="chest",coord={3,18},class="("})
des.monster({class="v",id="energy vortex",coord={58,3}})
des.monster({class="v",id="energy vortex",coord={64,2}})
des.monster({class="v",id="energy vortex",coord={65,4}})
des.monster({class="v",id="energy vortex",coord={61,5}})
if percent(50) then
des.monster({class="v",id="energy vortex",coord={61,1}})
end
des.monster({class="M",id="giant mummy",coord={55,4}})
des.monster({class="M",id="giant mummy",coord={53,5}})
des.monster({class="M",id="giant mummy",coord={56,6}})
des.monster({class="M",id="giant mummy",coord={57,8}})
if percent(50) then
des.monster({class="M",id="giant mummy",coord={59,10}})
end
des.monster({class=";",id="kraken",coord={43,4}})
des.monster({class="p",coord={49,9}})
des.monster({class="p",coord={41,10}})
des.monster({})
des.monster({})
if percent(50) then
des.monster({})
end
des.object({id="statue",coord={5,4},class="`",historic=true,montype="oread"})
-- Category 2: statue-only dryad -> oread. Both are nymph statue identities;
-- this decorative montype has no live-monster gameplay dependency here.
des.object({id="statue",coord={7,4},class="`",historic=true,montype="oread"})
-- Category 3: donor montype "god" is a non-species sentinel with no local
-- equivalent; preserve the named historic statue without a montype.
des.object({id="statue",coord={6,1},class="`",historic=true,name="Shub-Niggurath, Black Goat of the woods with a thousand young",contents=function()
  -- Category 1: donor +3 bone viperwhip is the existing local Step 10B
  -- viperwhip identity; preserve the donor name, blessing, and +3.
  des.object({id="viperwhip",buc="blessed",spe=3,name="bone viperwhip"})
  des.object({id="viperwhip",buc="blessed",spe=3,name="bone viperwhip"})
  des.object({id="viperwhip",buc="blessed",spe=3,name="bone viperwhip"})
end})
des.region({region={4,1,8,2},lit=1,type="temple"})
des.monster({class="t",id="mouth of the goat",coord={6,1}})
des.altar({coord={6,1},align="noalign",type="altar"})
-- Category 1: donor trapped metal box uses the existing local Step 10B
-- large box identity; preserve its display name, trap, curse, and +5.
des.object({id="large box",coord={74,1},name="trapped metal box",buc="cursed",spe=5,trapped=true})
des.monster({class="&",id="Hmnyw-Pharaoh",coord={74,1}})
-- Category 3: donor montype "god" is a non-species sentinel with no local
-- equivalent; preserve the named historic statue without a montype.
des.object({id="statue",coord={74,1},class="`",buc="uncursed",historic=true,name="the God of the Bloody Tongue",contents=function()
  des.object({id="death",class="/"})
  des.object({id="undead turning",class="/"})
end})
des.monster({class="@",id="coven leader",coord={69,1}})
-- Category 3: donor montype "god" is a non-species sentinel with no local
-- equivalent; preserve the named historic statue without a montype.
des.object({id="statue",coord={67,15},class="`",historic=true,name="Mother Hydra",contents=function()
  des.object({id="ruby",class="*"})
  des.object({id="ruby",class="*"})
  des.object({id="ruby",class="*"})
  des.object({id="ruby",class="*"})
  des.object({id="ruby",class="*"})
  des.object({id="ruby",class="*"})
  des.object({id="ruby",class="*"})
  des.object({id="ruby",class="*"})
end})
-- Category 3: donor montype "god" is a non-species sentinel with no local
-- equivalent; preserve the named historic statue without a montype.
des.object({id="statue",coord={67,15},class="`",historic=true,name="Father Dagon",contents=function()
  -- Category 1: donor gold amulet of magical breathing is the existing local
  -- Step 10B amulet of magical breathing identity; preserve its effect.
  des.object({id="amulet of magical breathing"})
  -- Category 2: star sapphire -> sapphire. The donor-only gem variant is
  -- absent locally; preserve the sapphire gem class and donor order. These
  -- are decorative statue contents, with no later map-logic dependency.
  des.object({id="sapphire",class="*"})
  des.object({id="sapphire",class="*"})
end})
-- Category 3: donor montype "god" has no local equivalent; preserve the
-- named historic statue without a montype.
des.object({id="statue",coord={38,10},class="`",historic=true,name="something long since forgotten"})
des.monster({class="&",id="priest of an unknown god",coord={38,11}})
des.monster({class="q",id="deep dweller",coord={64,14},asleep=true})
des.monster({class="h",id="deep one",coord={65,14},asleep=true})
if percent(50) then
des.monster({class="h",id="deep one",coord={65,16},asleep=true})
end
des.monster({class="h",id="deeper one",coord={70,15},asleep=true})
des.monster({class="q",id="deep dweller",coord={71,16},asleep=true})
des.object({id="chest",coord={69,12},class="("})
des.object({id="chest",coord={73,12},class="("})
des.object({id="create monster",coord={70,15},class="?",buc="cursed",spe=0})
des.object({coord={26,8},class="\""})
des.monster({class=";",coord={23,15}})
des.monster({class=";",coord={19,11}})
des.monster({class=";",coord={16,9}})
des.monster({class=";",coord={21,6}})
des.monster({class=";",coord={28,5}})
des.monster({class=";",coord={34,6}})
des.monster({class=";",id="electric eel",coord={38,8}})
des.monster({class=";",coord={43,12}})
des.monster({class=";",coord={50,15}})
if percent(50) then
des.monster({class=";",coord={58,14}})
end
if percent(50) then
des.monster({class=";",coord={65,9}})
end
if percent(50) then
des.monster({class=";",coord={72,6}})
end
des.object({class="!"})
des.object({class="!"})
des.object({class="!"})
des.object({class="/"})
des.object({class="*"})
des.object({class="%"})
des.object({class="%"})
des.object({class="%"})
des.object({class="%"})
des.object({})
des.object({})
des.object({})
des.map({halign="center",valign="center",map=[=[
0 ---------                                                                  
1 | S     |    eeeeeeeee                                                     
2 | |     |eeeeeeeeeeeeee                                                    
3 -----+---eeeeee eeeeeee                                                    
4    eeeeeeeeeeeeeeeeee}}}                                                   
5  eeeeeeeeeeeeeeeee}}}}}}                                                   
6 eeeee eeeee eeee}}}}}}}}                                                   
7 eeee eeeeeeeee}}}}}}}}                                                     
8  eeeeeeeeeeee}}}}}}}}                                                      
9   eeeeeee ee}}}}}}}}                                                       
0     ee eeeee}}}}}}}}}                                                      
1       eeeeeee}}}}}}}}}                                                     
2       #   eeee e}}}}}}                                                     
3      ##   # eeeeee}}}}                                                     
4     ##    #        }}}                                                     
5     #   #H##        }}}}                                                   
6        H#  ##       }}}}                                                   
7                     }}}}                                                   
8                    }}}}                                                    
9                   }}}}                                                     
]=]})
des.monster({class="h",id="small goat spawn"})
des.monster({class="h",id="small goat spawn"})
des.monster({class="h",id="small goat spawn"})
des.monster({class="h",id="small goat spawn"})
des.monster({class="h",id="small goat spawn"})
des.monster({class="@",id="goat spawn"})
des.monster({class="@",id="goat spawn"})
des.monster({class="@",id="goat spawn"})
des.monster({class="H",id="giant goat spawn"})
des.monster({class="C",id="plains centaur"})
des.monster({class="C",id="plains centaur"})
des.monster({class="C",id="plains centaur"})
des.monster({class="C",id="plains centaur"})
des.monster({class="C",id="plains centaur"})
des.monster({class="C",id="plains centaur"})
des.map({halign="center",valign="center",map=[=[
                                                                      ------
                                                         eeeeee-eeeeee|..  |
                                             #####   eeeeeee eeeee eee+..  |
                                         .....   ##eeeee eeeeeeeeeeeee------
                                         ..}..     ee eeeeeeee eeee eeee    
                                          ...       eeeeeeeeeeee eeee}}}}}}}
                                            ###      eeee eeeeeeeee}}}}}}}}}
                                              #       eeeeeee eee}}}}}}}}}}}
                                              ##...    eee eeee}}}}}}}}}}}} 
                                                ....    eeeee}}}}}}}}}}     
                                                 ....     ee}}}}}}}}        
                                                   ...    }}}}}}            
                                                         }}}}}              
                                                            }               
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
]=]})
des.monster({class="@",id="apprentice witch"})
des.monster({class="@",id="apprentice witch"})
des.monster({class="@",id="apprentice witch"})
des.monster({class="@",id="apprentice witch"})
des.monster({class="@",id="apprentice witch"})
des.monster({class="@",id="apprentice witch"})
des.monster({class="@",id="apprentice witch"})
des.monster({class="@",id="apprentice witch"})
des.monster({class="@",id="apprentice witch"})
des.monster({class="@",id="witch"})
des.monster({class="@",id="witch"})
des.monster({class="@",id="witch"})
des.monster({class="&",id="The Good Neighbor"})
des.map({halign="center",valign="center",map=[=[
 ---------                                                            ------
 |.S.....|    eeeeeeeee    ######                        eeeeee-eeeeee|....|
 |.|.....|eeeeeeeeeeeeeeH###    #######      #####   eeeeeee eeeee eee+....|
 -----+---eeeeee eeeeeee              ###.....   ##eeeee eeeeeeeeeeeee------
    eeeeeeeeeeeeeeeeee}}}}}}}}}}}}}}}    ..}..     ee eeeeeeee eeee eeee    
  eeeeeeeeeeeeeeeee}}}}}}}}}}}}}}}}}}}    ...       eeeeeeeeeeee eeee}}}}}}}
 eeeee eeeee eeee}}}}}}}}}}}}}}}}}}}}}}     ###      eeee eeeeeeeee}}}}}}}}}
 eeee eeeeeeeee}}}}}}}}  eeeeee }}}}}}}}      #       eeeeeee eee}}}}}}}}}}}
  eeeeeeeeeeee}}}}}}}}    ee  #  }}}}}}}}}    ##...    eee eeee}}}}}}}}}}}} 
   eeeeeee ee}}}}}}}}         ##  }}}}}}}}}}    ....    eeeee}}}}}}}}}}     
     ee eeeee}}}}}}}}}e        ###e}}}ee}}}}ee   ....     ee}}}}}}}}        
       eeeeeee}}}}}}}}}e           e}}}ee}}}}}ee # ...    }}}}}}    ------- 
       #   eeee e}}}}}}ee           e}}e e}}}}}eee       }}}}}      |.....| 
      ##   # eeeeee}}}}}e            ee}}ee}}}e}} ee    }}}}}  --------S--- 
     ##    #        }}}}ee       ###  }}e}}}}ee}}}}}e} }}}}}   |}}w{wwww|   
     #   #H##  ....  }}}}e#####  H ##  e}e}}}e}e}}}}}e}}}}}    |}w{.{ww}|   
    ....H#  ##.....  }}}}e    .. .  ###ee}e}e}}}e}}}}}e}}ee####+www{ww}}|   
  .....       ....   }}}}   ......      ee}e -}}}e}}}}}ee      ----------   
   .....       ...  }}}}    .....                                           
                   }}}}                                                     
]=]})
local place={{5,17},{2,2},{73,1},{71,12}}
shuffle(place)
des.region({region={0,0,75,19},lit=1,type="ordinary"})
des.region({region={2,1,2,2},lit=1,type="ordinary"})
des.region({region={2,16,7,18},lit=0,type="ordinary"})
des.region({region={14,15,18,18},lit=0,type="ordinary"})
des.region({region={28,16,33,18},lit=0,type="ordinary"})
des.region({region={41,3,45,5},lit=0,type="ordinary"})
des.region({region={48,8,53,11},lit=0,type="ordinary"})
des.region({region={71,1,74,2},lit=1,type="ordinary"})
des.region({region={64,14,71,16},lit=1,type="ordinary"})
des.region({region={69,12,73,12},lit=1,type="ordinary"})
des.stair({dir="down",coord=place[1]})
des.stair({dir="up",coord={30,17}})
des.door({state="closed",coord={6,3}})
des.door({state="locked",coord={3,1}})
des.door({state="closed",coord={70,2}})
des.door({state="locked",coord={63,16}})
des.door({state="closed",coord={71,13}})
des.trap("random")
des.trap("random")
des.trap("random")
des.trap("random")
des.trap("random")
if percent(7) then
des.monster({class="A",id="kuker"})
end
if percent(7) then
des.monster({class="A",id="kuker"})
end
if percent(7) then
des.monster({class="A",id="kuker"})
end
if percent(7) then
des.monster({class="A",id="kuker"})
end
