-- Step 10C-A fixed map resource: spire
-- Pinned donor: neutrality.des at commit 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0

des.level_flags("nommap","hardfloor","shortsighted")
des.level_init({style="mines",fg=".",bg="T",smoothed=false,joined=true,lit=false,walled=true})
-- Donor NOMAP marker is represented by the level_flags("nommap") contract.
des.monster({class="q"})
des.monster({class="q"})
des.monster({class="q"})
des.monster({class="q"})
des.monster({class="q"})
des.monster({class="q"})
des.monster({class="q"})
des.monster({class="q"})
des.monster({class="q"})
-- Donor class '"' is not a local monster class; identity remains name-resolved.
des.monster({id="Plumach Rilmani"})
des.monster({id="Plumach Rilmani"})
des.monster({id="Plumach Rilmani"})
des.monster({id="Plumach Rilmani"})
des.map({halign="center",valign="center",map=[=[
0TT......................................................TTTTTTTTTTTTTTTTTTTT
1T........................................................TTTTTTTTTTTTTTTTTTT
2TT......................................................TTTTTTTTTTTTTTTTTTTT
]=]})
-- Portal contract: region={1,1,1,1}, exclude={0,0,0,0}, destination="out4".
des.levregion({region={1,1,1,1},exclude={0,0,0,0},type="portal",name="out4"})
des.monster({id="Plumach Rilmani"})
des.monster({id="Plumach Rilmani"})
des.monster({id="Plumach Rilmani"})
des.monster({id="Plumach Rilmani"})
des.monster({class="'"})
des.monster({class="'"})
des.monster({class="'"})
des.monster({class="'"})
des.monster({class="'"})
des.monster({class="'"})
des.monster({class="'"})
des.monster({class="'"})
des.monster({class="'"})
des.monster({class="'"})
des.monster({class="'"})
des.monster({class="'"})
des.map({halign="right",valign="center",map=[=[
0TTTT|-----------------------------|TTTT
1TTT|-- - - - - - - - - - - - - - --|TTT
2TT|---------------------------------|TT
3TT|- - - - - - - - - - - - - - - - -|TT
4T|--------------.-------.------------|T
5T| - - - - - - ----------- - - - - - |T
6T|------------.-----------.----------|T
7|- - - - - - --------------- - - - - -|
8|.------------------------------------|
9-... - - - - --------------- - - - - .|
0.....-------------------------------..|
1-... - - - - --------------- - - - - .|
2|.------------------------------------|
3|- - - - - - --------------- - - - - -|
4T|------------.-----------.----------|T
5T| - - - - - - ----------- - - - - - |T
6T|--------------.-------.------------|T
7TT|- - - - - - - - - - - - - - - - -|TT
8TT|---------------------------------|TT
9TTT|-- - - - - - - - - - - - - - --||TT
0TTTT|-----------------------------|TTTT
]=]})
des.non_diggable(selection.area(0,0,38,20))
des.stair({dir="down",coord={37,10}})
des.mazewalk({coord={4,10},dir="east"})
des.monster({class="'",id="clay golem",coord={1,8}})
des.monster({class="'",id="clay golem",coord={1,12}})
des.monster({id="Ferrumach Rilmani"})
des.monster({id="Ferrumach Rilmani"})
des.monster({id="Ferrumach Rilmani"})
des.monster({id="Ferrumach Rilmani"})
