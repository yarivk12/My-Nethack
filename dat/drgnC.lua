-- My-NetHack modification notice, added 2026-10-15.
-- Local project integration/modifications; original authorship is retained.
-- Recorded project change dates (Git author dates): 2026-09-09.
-- See doc/public-source-compliance.md and doc/public-source-modifications.json.
-- Existing copyright, attribution, license and warranty notices still apply.
-- Restored donor notices from UnNetHack 439b8d63d3d1ca78fb08588dd43f61874114b21a
-- Source: dat/dragons.des
--       Copyright (c) 1989 by Jean-Christophe Collet
--       Copyright (c) 1991 by M. Stephenson
-- NetHack may be freely redistributed.  See license for details.
-- NetHack special level; see doc/step9b.md.
-- UnNetHack 439b8d63d3d1ca78fb08588dd43f61874114b21a:dat/dragons.des
des.level_init({style="solidfill",fg=" "})
des.map({halign="center",valign="center",map=[=[
PPPPPPPPPPP-        -----.......-      --MMM............---                 
PPPPPPP-----       --...........|     --MMM...............-----             
PPPPP---           |............|     |MMM....................---         --
PPPPP|            --...........--     |M.--.......-------.......--     ----M
PPP---            |..........---      ----|..T.----     --.......-------MMMM
P---              |......-----          ---.TT--         |..........MMMMMMMM
--        ----- ---....---            ---...TT|          --...............MM
        ---...---.....--              |T....T--          --.................
       --............--               |T..MM--          --..................
       |.............-----            --.MMM--        ---...................
---    |........--.......----         --MM..M--     ---T.......TT...........
..--   |......---|...MMMMMMM|        --MMM...M|   ---TTT.......TT.}}}.......
...|----......-----}}}}}}}}}--      --MM...MMM|   |TTT.........TTTTT}}......
...--...........| -----}}}}}}--     |M.....MM--  --..........}}}}}TT}}......
................|     ----}}}M--    |.....MM--  --............}}}}}}}MM.....
...............--        --}MMM--   |......------.....---...}}}TT}MM.MMM....
..............--          |..MMM|  --......---......-----..TTTTTTTTMMM......
---..........--           --...M----........TTT.....----.....MMMMTTMMM......
  ---.......--             --........--..............--.....MMMMMMMMMMMM....
    --....---               --......--|..TT....---.......MMMMMMMMMMMMMMM....
     ------                  -.....-- -...TTTT-- -.....MMMMMMTTTTTTTTT......
]=]})
des.stair("up")
des.stair("down")
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
