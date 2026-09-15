-- Step 10C-A fixed map resource: lbyrnth
-- Pinned donor: labr.des at commit 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0
-- Static body port only; topology, scheduler, and generator hooks remain deferred.
des.level_flags("noteleport","nommap")
des.level_init({style="mines",fg=" ",bg=" ",smoothed=false,joined=false,lit=false,walled=false})
des.map({halign="center",valign="center",map=[=[
0                         |---------------------------|                      
1                         |...|.....................|.|                      
2                         |.|.|.|-----------------|.|.|                      
3                         |.|.|.|.................|.|.|                      
4                         |.|.|.|.---------------.|.|.|                      
5                         |.|.|.|.|.............|.|.|.|                      
6                         |.|.|.|.|.-----------.|.|.|.|                      
7                         |.|.|.|.|.|.........|.|.|.|.|                      
8                         |.|.|.|.|.|.-------.|.|.|.|.|                      
9                         |.|.|.|.|.|.|.....|.|.|.|.|.|                      
0                         |.|.|.|.|.-.|.|-|.-.|.|.|.|.|                      
1                         |.|.|.|.|...|.|.|...|.|.|.|.|                      
2                         |.|.|.|.|----.|.-----.|.|.|.|                      
3                         |.|.|.|.......|.......|.|.|.|                      
4                         |.|.|.-----------------.|.|.|                      
5                         |.|.|.........|.........|.|.|                      
6                         |.|.-------S-.|.---------.|.|                      
7                         |.|.........|.|.|.........|.|                      
8                         |.----------|.|.|.---------.|                      
9                         |...........|...|...........|                      
0                         -----------------------------                      
]=]})
local place={{36,19},{40,11},{52,1}}
shuffle(place)
des.levregion({region={38,19,40,19},exclude={0,0,0,0},type="branch"})
des.non_diggable(selection.area(0,0,75,20))
des.non_passwall(selection.area(0,0,75,20))
des.monster({class="H",id="minotaur",coord=place[1]})
des.object({class="+",coord=place[1]})
des.object({class="+",coord=place[1]})
des.object({class="+",coord=place[1]})
des.monster({class="H",id="minotaur priestess",coord=place[2]})
des.object({class="+",coord=place[2]})
des.object({class="+",coord=place[2]})
des.object({class="+",coord=place[2]})
des.monster({class="@",id="Illurien of the Myriad Glimpses",coord=place[3]})
des.trap({type="rust"})
des.trap({type="rust"})
des.trap({type="rust"})
des.trap({type="rust"})
des.trap({type="rust"})
des.trap({type="rust"})
des.trap({type="rust"})
des.trap({type="rust"})
des.trap({type="rust"})
des.trap({type="rust"})
des.trap({type="rust"})
des.trap({type="rust"})
des.trap({type="rust"})
