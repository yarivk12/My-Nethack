-- Step 10C-A fixed map resource: lethe-g
-- Pinned donor: neutrality.des at commit 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0

des.level_flags("hardfloor")
des.map({halign="center",valign="center",map=[=[
0  ---------   ---     ---      ---      ---    ---    ---    ---      ------
1  |       |---- ------- -------- -------- ------ ------ ------ -------|    |
2  |                                                                        |
3  |       |---- -------}}------- -------- ------ ------ ------ -------|    |
4  ---------   - -     -}}}     ---      ---    - -    ---    ---      ------
5                    }}}}}}}}                                      }}}       
6                  }}}}}}}}}}}}}        ...  .........            }}}}}}...  
7                }}}}}}}}}}}}}}}}.      ...............          }}}}}}}}}.. 
8              }} }}}}}}}}}}}}}}}}....  ...   ...............   }}}}}}}}}}}} 
9           }}}}}} }       }}}}}}}}....       ........         }}}}}}}}}}}}}}
0       }}}}}}}}}           ..}}}}} }...       .....         }}}}}}}}}}}}}}}}
1  }}}}}}}}}}}     .         ...}} }}}}}         .         }} }}}}}}}}}}}}}}}
2}}}}}}}}}}}    ....          ....}}}}}}}}}}}          }}}}}}} }}}}}}}}....  
3}}}}}}}}}}   ...       ..     .   }}}}}}}}}}}}}}}}}}}}}}}}}}}} }..      .   
4}}}}}}}}}   ..      ......    ..  .}}}}}}}}}}}}}}}}}}}}}}}}}}....     ....  
5  }}}}    ...     ..........   .   .}}}}}}}}}}}}}}}}}}}}}}    .  .... ..    
6      ..... .    ...............   .}}}}}}}}}}}}}}}}}}}}}  ............ ..  
7      .     ...  ............    ....}}}}}}}}}}}}}}}}}}}} ............ .... 
8              ....  ..............  ....}}}}}}}}}}}}}}}}   ................ 
9                                          }}}}}}}}}}}}                      
]=]})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.map({halign="center",valign="center",map=[=[
0  ---------   ---     ---      ---      ---    ---    ---    ---      ------
1  |.......|----#-------}--------#--------#------#------#------#-------|....|
2  |.......+###########}}}}}}}}########################################+....|
3  |.......|----#-------}}-------#--------#------#------#------#-------|....|
4  ---------   -S-     -}}}     ---      ---    -S-    ---    ---      ------
5               H ...}}}}}}}}                    H                 }}}       
6               ...}}}}}}}}}}}}}     ###      .......             }}}}}}...  
7              ..}}}}}}}}}}}}}}}}.   H #####H..........        ..}}}}}}}}}.. 
8              }}.}}}}}}}}}}}}}}}}....        .........H##### ..}}}}}}}}}}}} 
9           }}}}}}.}...    }}}}}}}}....        ......       ...}}}}}}}}}}}}}}
0       }}}}}}}}}....       ..}}}}}.}...         H         ..}}}}}}}}}}}}}}}}
1  }}}}}}}}}}}     H         ...}}.}}}}}         H         }}.}}}}}}}}}}}}}}}
2}}}}}}}}}}}    ####          ....}}}}}}}}}}}  ......  }}}}}}}.}}}}}}}}....  
3}}}}}}}}}}   ###              #   }}}}}}}}}}}}}}}}}}}}}}}}}}}}.}..      H   
4}}}}}}}}}   ##        ...     ##  .}}}}}}}}}}}}}}}}}}}}}}}}}}....      ##   
5  }}}}... ###      ........    #   .}}}}}}}}}}}}}}}}}}}}}}    H       ##    
6    ....### H     ..........H###   .}}}}}}}}}}}}}}}}}}}}}  ..........H#     
7     ..     ###  #..........     ##..}}}}}}}}}}}}}}}}}}}} ...........   ..  
8              ####  ...  ...H#####  ....}}}}}}}}}}}}}}}}    .........H#...  
9                                          }}}}}}}}}}}}                      
]=]})
des.region({region={0,0,75,19},lit=1,type="ordinary"})
des.region({region={3,1,9,3},lit=1,type="ordinary"})
des.region({region={11,1,69,3},lit=0,type="ordinary"})
des.region({region={71,1,74,3},lit=1,type="ordinary"})
des.region({region={18,14,27,18},lit=0,type="ordinary"})
des.region({region={44,6,53,9},lit=0,type="ordinary"})
des.region({region={60,16,68,18},lit=0,type="ordinary"})
des.stair({dir="down",coord={73,2}})
des.stair({dir="up",coord={4,2}})
des.door({state="locked",coord={10,2}})
des.door({state="locked",coord={15,4}})
des.door({state="locked",coord={48,4}})
des.door({state="locked",coord={70,2}})
-- Category 2: statue-only elder priest -> high priest; preserve the local
-- priest role. No live-monster gameplay logic depends on this montype here.
des.object({id="statue",coord={3,2},class="`",historic=true,montype="high priest",contents=function()
  des.object({id="fortune cookie",class="%"})
end})
-- Category 2: statue-only lethe elemental -> water elemental; preserve the
-- river-elemental role. No live-monster gameplay logic depends on this
-- decorative montype here.
des.object({id="statue",coord={4,1},class="`",historic=true,montype="water elemental"})
des.object({id="statue",coord={4,3},class="`",historic=true,montype="water elemental"})
des.object({id="statue",coord={6,1},class="`",historic=true,montype="Angel"})
des.object({id="statue",coord={6,3},class="`",historic=true,montype="Angel"})
des.object({id="statue",coord={8,1},class="`",historic=true,montype="nightgaunt"})
des.object({id="statue",coord={8,3},class="`",historic=true,montype="nightgaunt"})
des.object({id="statue",coord={15,1},class="`",historic=true,montype="shoggoth"})
des.object({id="statue",coord={15,3},class="`",historic=true,montype="titanothere"})
des.object({id="statue",coord={23,1},class="`",historic=true,montype="Great Cthulhu",contents=function()
  des.object({id="aggravate monster",class="=",buc="cursed",spe=0})
end})
des.object({id="statue",coord={23,3},class="`",historic=true,montype="migo queen"})
des.object({id="statue",coord={32,1},class="`",historic=true,montype="Green-elf"})
des.object({id="statue",coord={32,3},class="`",historic=true,montype="orc"})
des.object({id="statue",coord={41,1},class="`",historic=true,montype="elf-lord"})
des.object({id="statue",coord={41,3},class="`",historic=true,montype="dwarf lord"})
-- Category 3: donor gnoll has no valid local equivalent;
-- preserve the historic statue without silently substituting a species.
des.object({id="statue",coord={48,1},class="`",historic=true})
des.object({id="statue",coord={48,3},class="`",historic=true,montype="kobold"})
des.object({id="statue",coord={55,1},class="`",historic=true,montype="knight"})
des.object({id="statue",coord={55,3},class="`",historic=true,montype="deepest one"})
des.object({id="statue",coord={62,1},class="`",historic=true,montype="hobbit"})
des.object({id="statue",coord={62,3},class="`",historic=true,montype="mind flayer"})
des.object({id="statue",coord={73,1},class="`",historic=true,montype="high priest"})
des.object({id="statue",coord={73,3},class="`",historic=true,montype="high priest"})
des.object({id="statue",coord={74,2},class="`",historic=true,montype="high priest"})
des.trap({type="pit",coord={21,2}})
des.trap({type="pit",coord={30,2}})
des.trap({type="board",coord={19,17}})
des.trap({type="board",coord={46,7}})
des.trap({type="board",coord={62,14}})
des.trap("random")
des.trap("random")
des.trap("random")
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.monster({class="F",id="migo worker",coord={48,7},asleep=true})
des.monster({class="F",id="migo worker",coord={48,7},asleep=true})
des.monster({class="F",id="migo worker",coord={48,7},asleep=true})
des.monster({class="F",id="migo worker",coord={48,7},asleep=true})
des.monster({class="F",id="migo worker",coord={48,7},asleep=true})
des.monster({class="F",id="migo worker",coord={48,7},asleep=true})
des.monster({class="F",id="migo worker",coord={48,7},asleep=true})
des.monster({class="F",id="migo worker",coord={48,7},asleep=true})
des.monster({class="F",id="migo worker",coord={48,7},asleep=true})
des.monster({class="F",id="migo philosopher",coord={23,16},asleep=true})
des.monster({class="F",id="migo philosopher",coord={23,16},asleep=true})
des.monster({class="F",id="migo philosopher",coord={23,16},asleep=true})
des.monster({class="F",id="migo soldier",coord={23,16},asleep=true})
des.monster({class="F",id="migo soldier",coord={23,16},asleep=true})
des.monster({class="F",id="migo soldier",coord={23,16},asleep=true})
des.monster({class="F",id="migo soldier",coord={23,16},asleep=true})
des.monster({class="F",id="migo soldier",coord={23,16},asleep=true})
des.monster({class="F",id="migo soldier",coord={23,16},asleep=true})
des.monster({class="F",id="migo soldier",coord={61,18},asleep=true})
des.monster({class="F",id="migo worker",coord={61,18},asleep=true})
des.monster({class="F",id="migo worker",coord={61,18},asleep=true})
des.monster({class="F",id="migo worker",coord={61,18},asleep=true})
des.monster({class="F",id="migo worker",coord={61,18},asleep=true})
des.monster({class="F",id="migo soldier",coord={61,18},asleep=true})
des.monster({class="F",id="migo soldier",coord={64,18},asleep=true})
des.monster({class="F",id="migo worker",coord={64,18},asleep=true})
des.monster({class="F",id="migo worker",coord={64,18},asleep=true})
des.monster({class="F",id="migo worker",coord={64,18},asleep=true})
des.monster({class="F",id="migo worker",coord={64,18},asleep=true})
des.monster({class="F",id="migo soldier",coord={64,18},asleep=true})
des.monster({class="F",id="migo queen",coord={67,18},asleep=true})
des.monster({class="F",id="migo philosopher",coord={67,18},asleep=true})
des.monster({class="F",id="migo philosopher",coord={67,18},asleep=true})
des.monster({class="F",id="migo soldier",coord={67,18},asleep=true})
des.monster({class="F",id="migo soldier",coord={67,18},asleep=true})
des.monster({class="F",id="migo soldier",coord={67,18},asleep=true})
des.monster({class="t",id="mouth of the goat",coord={73,18}})
des.object({id="full healing",coord={73,17},class="!"})
des.object({id="full healing",coord={73,17},class="!"})
des.object({id="full healing",coord={73,18},class="!"})
des.object({id="full healing",coord={73,18},class="!"})
des.object({id="gain energy",coord={73,18},class="!"})
-- Category 3: donor flying boots has no valid local equivalent. The donor
-- grants FLYING, whereas local levitation boots grant LEVITATION; omitting
-- the live object avoids a material movement-mechanics substitution.
-- Donor object contract: id="flying boots", coord={73,12}, class="[", buc="cursed", spe=0.
des.monster({class="t",coord={18,2}})
des.monster({class="t",coord={44,2}})
des.monster({class="t",coord={62,2}})
des.monster({class="B",coord={48,12}})
des.monster({class="B",coord={73,7}})
des.monster({class=";",coord={5,13}})
des.monster({class=";",coord={13,9}})
des.monster({class=";",coord={23,7}})
des.monster({class=";",coord={26,2}})
des.monster({class=";",coord={31,10}})
des.monster({class=";",coord={40,14}})
des.monster({class=";",coord={48,17}})
des.monster({class=";",coord={57,13}})
if percent(50) then
des.monster({class=";",coord={64,10}})
end
if percent(50) then
des.monster({class=";",coord={67,7}})
end
des.monster({})
des.monster({})
des.monster({})
if percent(7) then
des.monster({class="A",id="kuker"})
end
if percent(7) then
des.monster({class="A",id="kuker"})
end
