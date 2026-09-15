-- Step 10C-A fixed map resource: sumall
-- Pinned donor: neutrality.des at commit 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0

des.level_flags("nommap","hardfloor")
des.level_init({style="mines",fg=".",bg=" ",smoothed=true,joined=true,lit=true,walled=true})
-- Donor NOMAP marker is represented by the level_flags("nommap") contract.
-- Donor class '"' is not a local monster class; identity remains name-resolved.
des.monster({id="Plumach Rilmani"})
des.monster({id="Plumach Rilmani"})
des.monster({id="Plumach Rilmani"})
des.monster({id="Plumach Rilmani"})
des.monster({id="Plumach Rilmani"})
des.monster({id="Plumach Rilmani"})
des.monster({id="Plumach Rilmani"})
des.monster({id="Plumach Rilmani"})
des.monster({id="Plumach Rilmani"})
des.monster({id="Plumach Rilmani"})
des.monster({id="Plumach Rilmani"})
des.monster({id="Plumach Rilmani"})
des.monster({id="Ferrumach Rilmani"})
des.monster({id="Ferrumach Rilmani"})
des.monster({id="Ferrumach Rilmani"})
des.monster({id="Ferrumach Rilmani"})
des.monster({id="Ferrumach Rilmani"})
des.monster({id="Ferrumach Rilmani"})
des.monster({id="Ferrumach Rilmani"})
des.monster({id="Ferrumach Rilmani"})
des.monster({id="Cuprilach Rilmani"})
des.monster({id="Cuprilach Rilmani"})
des.monster({id="Cuprilach Rilmani"})
des.monster({id="Argenach Rilmani"})
des.map({halign="center",valign="center",map=[=[
0--.....--
1-...-...-
2..-...-..
3.........
4.-.....-.
5.........
6..-...-..
7-...-...-
8--.....--
]=]})
des.region({region={0,0,6,6},lit=1,type="ordinary"})
des.levregion({region={37,0,75,19},exclude={0,0,8,8},type="stair-up"})
des.levregion({region={4,4,4,4},exclude={0,0,0,0},type="branch"})
des.wallify()
des.monster({class="'",id="argentum golem",coord={2,4}})
des.monster({class="'",id="argentum golem",coord={3,3}})
des.monster({class="'",id="argentum golem",coord={4,2}})
des.monster({class="'",id="argentum golem",coord={5,3}})
des.monster({class="'",id="argentum golem",coord={6,4}})
des.monster({class="'",id="argentum golem",coord={5,5}})
des.monster({class="'",id="argentum golem",coord={4,6}})
des.monster({class="'",id="argentum golem",coord={3,5}})
