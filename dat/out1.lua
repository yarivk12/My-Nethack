-- Step 10C-A fixed map resource: out1
-- Pinned donor: neutrality.des at commit 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0

des.level_flags("shortsighted","hardfloor")
des.level_init({style="mines",fg="T",bg="G",smoothed=false,joined=false,lit=true,walled=false})
-- Donor NOMAP marker is represented by the level_flags("nommap") contract.
-- Step 10C-C: the engine invokes place_neutral_features() after this map loads.
-- Portal contract: region={0,0,75,20}, exclude={0,0,0,0}, destination="gatetwn".
des.levregion({region={0,0,75,20},exclude={0,0,0,0},type="portal",name="gatetwn"})
-- Portal contract: region={0,0,75,20}, exclude={0,0,0,0}, destination="out2".
des.levregion({region={0,0,75,20},exclude={0,0,0,0},type="portal",name="out2"})
if percent(10) then
des.monster({class="u",id="gray unicorn"})
end
des.monster({class="q"})
des.monster({class="q"})
des.monster({class="C",id="plains centaur"})
des.monster({class="C",id="plains centaur"})
des.monster({class="C",id="plains centaur"})
des.monster({class="C",id="plains centaur"})
des.monster({class="C",id="plains centaur"})
des.monster({class="C",id="plains centaur"})
