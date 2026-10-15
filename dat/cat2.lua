-- My-NetHack modification notice, added 2026-10-15.
-- Local project integration/modifications; original authorship is retained.
-- Recorded project change dates (Git author dates): 2026-09-09.
-- See doc/public-source-compliance.md and doc/public-main-modifications.json.
-- Existing copyright, attribution, license and warranty notices still apply.
-- Restored donor notices from dNetHack 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0
-- Source: dnethack-3.4.3/dat/chaos2.des
-- NetHack may be freely redistributed.  See license for details.
-- NetHack 5.0 Lua conversion; see doc/step9c.md.
-- dNetHack 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0:dnethack-3.4.3/dat/chaos2.des
des.level_init({style="solidfill",fg=" "})
des.level_flags("shortsighted","hardfloor")
des.map({halign="center",valign="center",map=[=[
 -------------
 |...........|
 |...........|
 |...........|
 |...........|
||...........|
|......-.....|
||...........|
 |...........|
 |...........|
 |...........|
 |...........|
 -------------
]=]})
des.levregion({region={1,6,1,6},exclude={0,0,0,0},type="portal",name="cat1"})
des.object({id="skeleton key",name="The Second Key of Chaos",buc="uncursed",spe=0})
des.monster({id="first wraithworm",coord={7,6}})
des.levregion({region={7,0,12,12},exclude={0,0,0,0},type="portal",name="cat3"})
