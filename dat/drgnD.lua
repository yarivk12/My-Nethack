-- NetHack special level; see doc/step9b.md.
-- UnNetHack 439b8d63d3d1ca78fb08588dd43f61874114b21a:dat/dragons.des
des.level_init({style="solidfill",fg=" "})
des.level_flags("noteleport","hardfloor")
des.map({halign="center",valign="center",map=[=[
         -ttttt.......TTTtttTTtt--     -T}}}}}}}}}M----TTTTttttTTTTT--      
-------  |TT.T..........TTTTT....----  |}}}}}}}}MMM||.TTTTTTttTTTTT..---    
|.TttT-- --................TT.......-----}}}}}}MM.---....TTTTTT........---  
|..TT..| --..........TT...............---}}}}}}M..||.......T.............---
-..T...|--.....T........................}}}}}}MM..--.................T.....|
T.TT...--TT...TTT.........---...........}}}}}}MM..............M............|
...Tt..................TTT|--..........}}}}}}}MM......--......MM....T......|
T...t...............-------|..........}}}}}}}}M......-------...MM..........-
....T...TT---.....---      -----.....}}}}}}}}......--- -----...PPPM.....TT..
T.......T----.....------  -------...}}}}}}}}.......-----.........PPP.....T..
-T.......|--T........TT----...---...}}}}}}}}......................MM.....TT.
|TT....---|..T.......TTTTT.......M.}}}}}}}}..----TT................M........
--------  --........TTT..........M}}}}}}}}}-------........TTTT..............
-------    --.......T.P.........MM}}}}}}}}}----T----.....TTTT...............
-T....---------.....PPPM.......MM}}}M}}}}}}.......|--....TT......--T......--
T.......--.TT--...MPPPPM......MM}}}}M}}}}}}......----...........--|T......--
T................MM..MPP.....MMM}}}}..}}}}}......---............|--TT......-
.....--...............PPPTTTMMM}}}}}M.}}}}.....................--|.T........
.....--...T............TTTTTMM}}}}}MM.}}}}...........T........-- |..........
-.........TT..........TttTTMMM}}}}MM.}}}}}..........TT.......--  --.........
---.....TTTTTT...TTTTTttTTMMM}}}}MM.}}}}}}......tttTtTTTTT.---    ----......
]=]})
des.stair("up",73,18)
des.object({class="*",coord={1,2}})
des.gold({amount=600 + d(12,100),coord={1,2}})
if percent(60) then des.object({id="gain ability",coord={1,3}}) end
if percent(80) then des.object({class="=",coord={1,3}}) end
if percent(40) then des.object({class="\"",coord={1,3}}) end
des.object({class="*",coord={1,3}})
des.gold({amount=600 + d(10,100),coord={1,3}})
if percent(60) then des.object({id="gain level",coord={2,3}}) end
if percent(80) then des.object({class="=",coord={2,3}}) end
if percent(40) then des.object({class="\"",coord={2,3}}) end
des.object({class="*",coord={2,3}})
des.gold({amount=600 + d(10,100),coord={2,3}})
if percent(60) then des.object({id="full healing",coord={1,4}}) end
if percent(80) then des.object({class="=",coord={1,4}}) end
if percent(40) then des.object({class="\"",coord={1,4}}) end
des.object({class="*",coord={1,4}})
des.gold({amount=600 + d(10,100),coord={1,4}})
if percent(60) then des.object({id="enlightenment",coord={2,4}}) end
if percent(80) then des.object({class="=",coord={2,4}}) end
if percent(40) then des.object({class="\"",coord={2,4}}) end
des.object({class="*",coord={2,4}})
des.gold({amount=600 + d(10,100),coord={2,4}})
des.gold({amount=400 + d(10,100),coord={1,5}})
des.gold({amount=200 +  d(5,100),coord={0,6}})
des.gold({amount=200 +  d(5,100),coord={1,6}})
des.gold({amount=200 +  d(5,100),coord={2,6}})
des.gold({amount=100 +  d(5,100),coord={2,7}})
for i=1,d(2,10) do
des.gold({amount=d(1,100)})
end
des.object({id="boulder",coord={1,5}})
for i=1,12 do
des.object({class="*"})
end
for i=1,3 do
des.object({class="("})
des.object({class=")"})
des.object({class="!"})
des.object({class="?"})
end
for i=1,5 do
des.object()
end
for i=1,22 do
des.monster({class="D",peaceful=false})
end
des.monster({id="chromatic cave dragon",coord={1,7},peaceful=false})
des.monster({id="chromatic cave dragon",coord={2,6},peaceful=false})
des.monster({id="chromatic cave dragon",coord={1,6},peaceful=false})
for i=1,6 do
des.monster({class="w",peaceful=false})
end
for i=1,4 do
des.trap()
end
