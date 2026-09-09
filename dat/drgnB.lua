-- NetHack special level; see doc/step9b.md.
-- UnNetHack 439b8d63d3d1ca78fb08588dd43f61874114b21a:dat/dragons.des
des.level_init({style="solidfill",fg=" "})
des.map({halign="center",valign="center",map=[=[
.......---         ----......-              -----.................-         
.........---      --.........|  ----  -------M.................----         
...........--------.........-- --TT----PPPPMMM................--            
............................----TTPPPPPPPPMM..............T.---             
...........................PPPPPPPPPPPPPPPMM.............TT--               
.......................PPPPPPPPPPPPPPPPPPPM............TTTT--               
....................PPP..PPPPPPPPPPP|PPPPMM.............TTTT---             
..................PP....-----PPPPPP---PPPP..................TT--            
................PP.....--   --TPPPP| ----P.....................|----------  
..-------.....PP......--    ----PP--    ---....................--.|......|  
---     --...P........|     |TTT---   -----.......................+......|  
     --- --PP.........|     -----   ---...........................|......|  
     |P---PPP.........---         ---............................---------  
------PPPPPP............|        --TT...................TTT....T--          
PPPPPPPPPPPP............|       --TT...................TTT...TTT|           
PPPP----PPTT............--   ----T..................TTTTTT.--TT--           
PPP--  --TTT.............-----TTT................-----TTTTT|----            
PP--    --TTTT..............................------   -------                
---      --T..............................---                               
          --.............................--                                 
           --------.....................--                                  
]=]})
des.non_diggable(selection.area(64,6,75,15))
-- The donor compiler treats the unknown "dragon shop" name as OROOM.
des.region({region={67,9,72,11},lit=1,type="ordinary"})
des.door("open",66,10)
des.levregion({region={0,0,63,20},exclude={0,0,0,0},type="stair-up"})
des.levregion({region={0,0,63,20},exclude={0,0,0,0},type="stair-down"})
for i=1,3 do
des.object({class="*"})
des.object()
end
des.object({class="("})
for i=1,14 do
des.monster({class="D",peaceful=false})
end
des.monster({class="D",peaceful=false})
des.monster({class="D",peaceful=false})
des.monster({class="D",peaceful=false})
for i=1,4 do
des.monster({class="w",peaceful=false})
end
for i=1,4 do
des.trap()
end
