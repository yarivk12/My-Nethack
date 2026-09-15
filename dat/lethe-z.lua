-- Step 10C-A fixed map resource: lethe-z
-- Pinned donor: neutrality.des at commit 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0

des.map({halign="center",valign="center",map=[=[
0                                                        ---------           
1        .....                                           |.......|    }      
2      ..... ...                                   }     |.......|   }}}     
3     ......... .......... ..              .      .}}    |.......|  .}}      
4    .. ........................           ...   ...}}}  |.......| ...}      
5    ...... ....          . ..... ..      .. .. .....}...|.......|....       
6-----........    -------      ......... ...... .........--+-+-+--....       
7|...|......      |..\..|         .. ........-----...................        
8|...+.....   ----|.....---------  ....... ..|   |................-----------
9|...|...     |...+.....+.......|   .........-----........}.......|.....|...|
0-----....    |...------|.......|     ... ...............}}}......+.....S.}.|
1    .....H###S...+.....|.......+####....................}}.......|.....|...|
2    . ...    |...|\....+.......|    .. .....-----.........}......-----------
3   .......   ----|.....|.......|     ..... .|   |.............}}}}}}}}.     
4}}}}}}}}}}}}     ---------------   }}}}}}}}}-----}}}}}}}}}}}}}}}}}}}}}}}.   
5}}}}}}}}}}}}}}                   }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}.  
6}}}}}}}}}}}}}}}}}}}           }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}.  
7        }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}-----}}}}}}}}}}}}}}}}}}}}}}}.   
8            }}}}}}}}}}}}}}}}}}}}}}}}        |   |             }}}}}}}}.     
9               }}}}}}}}}}}}}}}}}}}          -----                           
]=]})
des.trap({type="hole",coord={70,13}})
des.trap({type="hole",coord={72,14}})
des.trap({type="hole",coord={73,15}})
des.trap({type="hole",coord={73,16}})
des.trap({type="hole",coord={72,17}})
des.trap({type="hole",coord={70,18}})
des.region({region={0,0,46,19},lit=1,type="ordinary"})
des.region({region={47,0,75,19},lit=0,type="ordinary"})
des.region({region={1,7,3,9},lit=1,type="ordinary"})
des.region({region={14,9,16,12},lit=0,type="ordinary"})
des.region({region={18,7,22,9},lit=0,type="ordinary"})
des.region({region={18,11,22,13},lit=0,type="ordinary"})
des.region({region={24,9,29,13},lit=0,type="barracks",filled=1})
des.region({region={59,1,63,5},lit=0,type="morgue",filled=1})
des.region({region={66,9,70,11},lit=0,type="ordinary"})
des.region({region={72,9,74,11},lit=0,type="ordinary"})
des.levregion({region={74,10,74,10},exclude={0,0,0,0},type="branch"})
des.stair({dir="up",coord={2,8}})
des.door({state="locked",coord={4,8}})
des.door({state="locked",coord={13,11}})
des.door({state="closed",coord={17,9}})
des.door({state="closed",coord={17,11}})
des.door({state="closed",coord={23,9}})
des.door({state="closed",coord={23,12}})
des.door({state="closed",coord={31,11}})
des.door({state="closed",coord={58,6}})
des.door({state="closed",coord={60,6}})
des.door({state="closed",coord={62,6}})
des.door({state="locked",coord={65,10}})
des.door({state="locked",coord={71,10}})
des.monster({class=";",coord={3,15}})
des.monster({class=";",coord={9,15}})
des.monster({class=";",coord={15,17}})
des.monster({class=";",coord={24,18}})
des.monster({class=";",coord={33,17}})
des.monster({class=";",coord={40,15}})
des.monster({class=";",coord={51,15}})
des.monster({class=";",coord={59,16}})
des.monster({class=";",coord={64,14}})
des.monster({class=";",coord={68,16}})
des.monster({class="h",id="deep one",coord={7,12}})
des.monster({class="h",id="deep one",coord={8,7}})
des.monster({class="h",id="deep one",coord={5,4}})
des.monster({class="h",id="deep one",coord={16,4}})
des.monster({class="h",id="deep one",coord={24,3}})
if percent(50) then
des.monster({class="h",id="deeper one",coord={31,6}})
end
des.object({id="amnesia",coord={20,3},class="!"})
des.object({id="amnesia",coord={4,11},class="!"})
des.object({id="fire",coord={5,4},class="/"})
des.object({id="lightning",coord={13,5},class="/"})
des.object({id="create monster",coord={27,4},class="?"})
des.monster({class="g",id="nightgaunt",coord={67,4},asleep=true})
des.monster({class="g",id="nightgaunt",coord={68,5},asleep=true})
des.monster({class="&",id="horned devil",coord={53,7},asleep=true})
des.monster({class="&",id="horned devil",coord={54,9},asleep=true})
if percent(60) then
des.monster({class="g",id="nightgaunt",coord={54,11},asleep=true})
end
if percent(60) then
des.monster({class="&",id="horned devil",coord={52,13},asleep=true})
end
if percent(60) then
des.monster({class="&",id="horned devil",coord={50,6},asleep=true})
end
des.monster({class="&",id="barbed devil",coord={51,9},asleep=true})
des.monster({class="&",id="barbed devil",coord={51,12},asleep=true})
if percent(75) then
des.monster({class="h",id="deeper one",coord={48,10},asleep=true})
end
if percent(75) then
des.monster({class="h",id="deeper one",coord={48,11},asleep=true})
end
if percent(75) then
des.monster({class="h",id="deeper one",coord={51,10},asleep=true})
end
des.monster({class="h",id="deep one",coord={18,9},asleep=true})
des.monster({class="h",id="deep one",coord={19,9},asleep=true})
des.monster({class="h",id="deep one",coord={20,9},asleep=true})
des.monster({class="h",id="deep one",coord={21,9},asleep=true})
des.monster({class="h",id="deep one",coord={22,9},asleep=true})
des.monster({class="h",id="deeper one",coord={20,7},asleep=true})
-- Category 1: donor Sword of the Deeps has the existing local long sword base
-- identity, but this named map-owned instance belongs to Step 10C-D.
-- Donor object contract: id="cursed +12 deep long sword named The Sword of the Deeps", coord={20,7}, buc="cursed", spe=12.
des.object({id="long sword",coord={20,7},class=")",buc="cursed",spe=12,deep=true,name="The Sword of the Deeps"})
des.monster({class="h",id="deep one",coord={19,11},asleep=true})
des.monster({class="h",id="deep one",coord={21,12},asleep=true})
des.monster({class="h",id="deep one",coord={22,13},asleep=true})
des.monster({class="h",id="deep one",coord={19,13},asleep=true})
des.monster({class="h",id="deep one",coord={18,12},asleep=true})
des.object({coord={19,11},class="/"})
des.object({coord={21,12},class="/"})
des.object({coord={22,13},class="/"})
des.object({coord={19,13},class="/"})
des.object({id="sleep",coord={18,12},class="/"})
des.object({id="lightning",coord={18,12},class="/"})
des.object({id="amulet of life saving",coord={18,12},class="\""})
des.object({id="protection",coord={18,12},class="=",buc="blessed",spe=3})
des.monster({class="h",id="deep one",coord={15,9},asleep=true})
des.monster({class="h",id="deep one",coord={15,10},asleep=true})
des.monster({class="h",id="deep one",coord={15,11},asleep=true})
des.monster({class="h",id="deep one",coord={15,12},asleep=true})
des.monster({class="h",id="deep one",coord={16,10},asleep=true})
des.object({id="statue",coord={1,8},class="`",historic=true,name="Shudde M'ell",montype="purple worm",contents=function()
  des.object({id="drum of earthquake",class="(",buc="cursed",spe=8})
end})
des.object({id="statue",coord={59,10},class="`",historic=true,montype="knight",contents=function()
  des.object({id="long sword",class=")",buc="blessed",spe=2})
end})
des.object({id="statue",coord={58,9},class="`",historic=true,montype="Angel",contents=function()
  des.object({id="full healing",class="!",buc="blessed",spe=0})
end})
des.object({id="statue",coord={58,11},class="`",historic=true,montype="pit fiend",contents=function()
  des.object({id="taming",class="?",buc="blessed",spe=0})
end})
des.object({id="corpse",coord={52,8},class="%",montype="knight",spe=0})
des.object({id="corpse",coord={61,12},class="%",montype="wizard",spe=0})
des.object({id="corpse",coord={65,7},class="%",montype="rogue",spe=0})
des.object({id="corpse",coord={62,8},class="%",montype="aligned cleric",spe=0})
des.object({id="corpse",coord={56,13},class="%",montype="ranger",spe=0})
des.monster({class="B",id="vampire bat"})
des.monster({class="B",id="vampire bat"})
if percent(50) then
des.monster({class="B",id="vampire bat"})
end
des.monster({class="B",id="raven"})
if percent(50) then
des.monster({class="B",id="raven"})
end
des.monster({class="B"})
des.monster({class="B"})
if percent(66) then
des.monster({class="B"})
end
if percent(33) then
des.monster({class="B"})
end
des.trap("spiked pit")
des.trap("spiked pit")
des.trap("rust")
des.trap("rust")
des.trap("magic")
des.trap("magic")
des.trap("magic")
des.trap({type="board",coord={60,8}})
des.trap({type="board",coord={64,10}})
des.trap({type="board",coord={46,10}})
des.trap({type="board",coord={46,11}})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
des.object({})
if percent(7) then
des.monster({class="A",id="kuker"})
end
if percent(7) then
des.monster({class="A",id="kuker"})
end
