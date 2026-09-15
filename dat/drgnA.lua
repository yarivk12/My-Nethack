-- NetHack special level; see doc/step9b.md.
-- UnNetHack 439b8d63d3d1ca78fb08588dd43f61874114b21a:dat/dragons.des
des.level_init({style="solidfill",fg=" "})
des.map({halign="center",valign="center",map=[=[
                          -----...........---                 -----.........
                        ---.................--             ----......TTTTTT.
                      ---............TTTT....--         ----...........TT...
                     --................TTT....--    -----...................
                     |.....TTT............T....------.......................
                    --...TTTT.....TT......T....................TT..........-
                    |......TT...............................TTTTTT........--
   ---------        |................T.........................TTT......--- 
----TTTT...-----   --...................................................|   
TTTTT..........-----T..................------..........................--   
.........TT.......TTT................---    ----.......-----...........--   
......TTT.T.........................--         ---------   ---..........--  
..........TTT.................T....--                       --...........-- 
...........TTTTT..........tttTT....|                    -----............T| 
..............T..........ttTTT....--            ---------TTTT.........TTTT| 
........................tttTT.....|            --M...tttt.............T---- 
.................................--          ---MM................------    
............T.---...............--          --PMMM............-----         
...........T--- -----....ttt.----          --PPPMMTTT.........-----         
....TTTT.TT--       ----------             |PPPPPMMTTT............-----     
......TTTT--                               -PPPPPPPTTTT...............-     
]=]})
des.stair("up")
des.stair("down")
-- Donor Valley portal omitted; return through the DoD parent branch stair.
des.object({class="*"})
des.object({class="*"})
des.object({class="*"})
des.object({class="("})
des.object()
des.object()
des.object()
for i=1,14 do
des.monster({class="D",peaceful=false})
end
des.monster({class="D",peaceful=false})
des.monster({class="D",peaceful=false})
for i=1,4 do
des.monster({class="w",peaceful=false})
end
for i=1,4 do
des.trap()
end
