-- Step 10C-A fixed map resource: leth-d-2
-- Pinned donor: neutrality.des at commit 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0

des.level_flags("hardfloor")
des.map({halign="center",valign="center",map=[=[
 ------------------------------------   }}}}              -----             
 |.............}...........}........|    }}}}             |...|             
 |}}}}..}}}}}..}}}..}}}}}..}}}..}}..|  ..}}}}..           |...|             
 |......}...}...........}..}.....}..| ...}}}}...          --+--         ..  
 |..}}}}}}..}..}}}}}}}..}.....{..}..|...}}}}}}...|  -----   #          .... 
 |..........}..}.....}}}}..}.....}..|..}}}}}}}}..|  |...|   #        ##.... 
 |}}}}..}}}}}..}}}}..}..}..}}}..}}..+..}}}}}}}}..+##S...+####H###    # .... 
 |..........}........}.....}........|..}}}}}}}}..|  |...|       ###  #  ..  
 |..........-----------------------S|...}}}}}}...|  -----         ####      
 |....}}....|.........|..|.|..|.....-------------------  ....          #    
 |....}}....|.........|+--+--+|.....|..........|.|....| ..}}}III       H    
 |....}}....|.........|.......|.....|........\...|....S#.}}}I}}}IIII   ..   
 |....}}....|.........|.......|.....|..........|.S....| .}}}}}}}}}}}I ....  
 |....}}....-----+--------+------+---..........-----+--  }I}}}}}}}}}}II}}}}}
 |....}}....+.......................+.........\..| |.|    }}}}}I..II}}}}}I}}
 |....}}....-----+--------+------+---..........-----+--      .......}}I}}}}I
 |....}}....|.........|.......|.....|..........|.S....|          #          
 |....}}....|.........|.......|.....|........\...|....S###########          
 |..........|.........|.......|.....|..........|.|....|                     
 ------------------------------------------------------                     
]=]})
des.region({region={2,1,35,7},lit=1,type="swamp",filled=1})
des.region({region={2,8,11,18},lit=1,type="ordinary"})
des.region({region={37,1,48,8},lit=1,type="ordinary"})
des.region({region={53,5,55,7},lit=1,type="ordinary"})
des.region({region={59,1,61,2},lit=1,type="ordinary"})
des.region({region={13,9,21,11},lit=1,type="morgue",filled=1})
des.region({region={13,16,21,18},lit=1,type="ordinary"})
des.region({region={23,11,29,12},lit=0,type="ordinary"})
des.region({region={23,9,24,9},lit=0,type="ordinary"})
des.region({region={26,9,26,9},lit=0,type="ordinary"})
des.region({region={28,9,29,9},lit=0,type="ordinary"})
des.region({region={23,16,29,18},lit=1,type="ordinary"})
des.region({region={31,9,35,12},lit=1,type="ordinary"})
des.region({region={31,16,35,18},lit=1,type="ordinary"})
des.region({region={13,14,35,14},lit=1,type="ordinary"})
des.region({region={38,10,45,13},lit=1,type="morgue",filled=1})
des.region({region={38,15,45,18},lit=1,type="morgue",filled=1})
des.region({region={48,10,48,12},lit=1,type="ordinary"})
des.region({region={47,14,48,14},lit=1,type="ordinary"})
des.region({region={48,16,48,18},lit=1,type="ordinary"})
des.region({region={50,10,53,12},lit=0,type="ordinary"})
des.region({region={50,16,53,18},lit=0,type="ordinary"})
des.region({region={52,14,52,14},lit=0,type="ordinary"})
des.region({region={56,9,75,15},lit=1,type="ordinary"})
des.region({region={71,3,74,7},lit=0,type="ordinary"})
des.stair({dir="down",coord={73,5}})
des.stair({dir="up",coord={60,1}})
des.door({state="locked",coord={60,3}})
des.door({state="locked",coord={56,6}})
des.door({state="locked",coord={52,6}})
des.door({state="locked",coord={49,6}})
des.door({state="locked",coord={36,6}})
des.door({state="locked",coord={35,8}})
des.door({state="random",coord={12,14}})
des.door({state="random",coord={17,13}})
des.door({state="random",coord={17,15}})
des.door({state="random",coord={26,13}})
des.door({state="random",coord={26,15}})
des.door({state="random",coord={23,10}})
des.door({state="random",coord={26,10}})
des.door({state="random",coord={29,10}})
des.door({state="locked",coord={33,13}})
des.door({state="random",coord={33,15}})
des.door({state="locked",coord={36,14}})
des.door({state="locked",coord={49,16}})
des.door({state="locked",coord={49,12}})
des.door({state="locked",coord={54,11}})
des.door({state="locked",coord={54,17}})
des.door({state="random",coord={52,13}})
des.door({state="random",coord={52,15}})
des.monster({class="a",id="killer bee",coord={3,15}})
des.monster({class="a",id="killer bee",coord={4,15}})
des.monster({class="a",id="killer bee",coord={3,16}})
des.monster({class="a",id="killer bee",coord={4,16}})
des.monster({class="M",id="giant mummy",coord={10,13},asleep=true})
des.monster({class="M",id="giant mummy",coord={10,15},asleep=true})
des.monster({class="F",id="shrieker",coord={3,11},asleep=true})
des.monster({class="F",id="shrieker",coord={10,11},asleep=true})
des.monster({class="F",id="shrieker",coord={3,14},asleep=true})
des.monster({class="F",id="shrieker",coord={10,14},asleep=true})
des.monster({class="F",id="shrieker",coord={3,17},asleep=true})
des.monster({class="F",id="shrieker",coord={10,17},asleep=true})
des.trap({type="polymorph",coord={35,9}})
des.trap({type="board",coord={35,14}})
des.trap({type="board",coord={35,14}})
des.trap({type="magic",coord={34,6}})
des.trap({type="magic",coord={53,17}})
des.monster({class=";",id="giant eel",coord={42,2}})
des.monster({class=";",id="giant eel",coord={44,6}})
des.monster({class=";",id="shark",coord={41,7}})
des.monster({class=";",id="giant eel",coord={59,12}})
des.monster({class=";",id="electric eel",coord={66,12}})
des.monster({class=";",id="kraken",coord={71,15}})
des.monster({class="D",id="black dragon",coord={26,11},asleep=true})
des.object({id="chest",coord={24,9},class="("})
des.object({id="chest",coord={26,9},class="("})
des.object({id="chest",coord={28,9},class="("})
des.monster({class="L",id="master lich",coord={45,11},asleep=true})
des.object({id="lightning",coord={45,11},class="/"})
des.object({id="chest",coord={48,12},class="("})
des.monster({class="L",id="alhoon",coord={46,14}})
des.object({id="cancellation",coord={46,14},class="/"})
des.object({id="secrets",coord={46,14},class="+",name="Necronomicon",spe=0})
des.trap({type="magic",coord={47,14}})
des.object({id="chest",coord={48,14},class="("})
des.monster({class="&",id="succubus",coord={46,13},asleep=true})
des.monster({class="&",id="succubus",coord={46,15},asleep=true})
des.monster({class="L",id="master lich",coord={45,17},asleep=true})
des.object({id="sleep",coord={45,17},class="/"})
des.object({id="chest",coord={48,16},class="("})
des.object({id="statue",coord={47,8},class="`"})
des.object({id="statue",coord={38,8},class="`"})
des.object({id="statue",coord={26,18},class="`"})
des.monster({})
des.monster({})
des.monster({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.trap("random")
des.trap("random")
des.trap("random")
des.trap("random")
des.trap("random")
des.trap("random")
if percent(80) then
des.monster({class="H",id="lurking one",coord={38,3}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={37,8}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={48,8}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={46,2}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={73,12}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={61,15}})
end
if percent(80) then
des.monster({class="H",id="lurking one",coord={57,9}})
end
