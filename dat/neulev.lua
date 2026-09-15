-- Step 10C-A fixed map resource: neulev
-- Pinned donor: neutrality.des at commit 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0

des.level_flags("nommap")
des.map({halign="center",valign="center",map=[=[
0----------------------------------------------------------------------------
1|...+##################################################################+...|
2|...---------------------------------++---------------------------------...|
3|...|......T......................PPPBBPPP.........................T...|...|
4|+---..T.w....PPPPPPPPPPPPPPPPPPPPP--++--PPPPPPPPPPPPPPPPPPPPP.........---+|
5|#|...........P--F--F..............F....F..............F--F--P...T..w....|#|
6|#|.T.........PF....--S--------------..--------------S--....FP.......T...|#|
7|#|....w..T...P-....+#####+....+############+....+#####+....-P.T...T.....|#|
8|#|...........P-....-------....------##------....-------....-P...........|#|
9|#|....T.w....}-FFFF-.....S....-.K..-##-....-....+.....-----FP..T...T....|#|
0|#+.................+.....------....+##+....------....\-{}PPPP..w........+#|
1|#|..T........}-FFFF-.....S....-....-##-..K.-....+.....-----FP.....T.....|#|
2|#|......T....P-....-------....------##------....-------....-P.....w.....|#|
3|#|...........P-....+#####+....+############+....+#####+....-P..T....T...|#|
4|#|T..T.......PF....--S--------------..--------------S--....FP...........|#|
5|#|.....w.....P--F--F..............F....F..............F--F--P.w..T......|#|
6|+---....T....PPPPPPPPPPPPPPPPPPPPP--++--PPPPPPPPPPPPPPPPPPPPP.........---+|
7|...|.w.....T.....................PPPBBPPP....................T.....T..|...|
8|...---------------------------------++---------------------------------...|
9|...+##################################################################+...|
0----------------------------------------------------------------------------
]=]})
local place={{2,2},{2,18},{73,2},{73,18}}
shuffle(place)
des.stair({dir="up",coord=place[1]})
des.stair({dir="down",coord=place[2]})
des.levregion({region={4,3,71,17},exclude={14,4,61,16},type="branch"})
des.door({state="closed",coord={2,10}})
des.door({state="closed",coord={73,10}})
des.door({state="closed",coord={36,10}})
des.door({state="closed",coord={39,10}})
des.door({state="closed",coord={37,2}})
des.door({state="closed",coord={38,2}})
des.door({state="locked",coord={37,4}})
des.door({state="locked",coord={38,4}})
des.door({state="locked",coord={37,16}})
des.door({state="locked",coord={38,16}})
des.door({state="closed",coord={37,18}})
des.door({state="closed",coord={38,18}})
des.door({state="locked",coord={55,13}})
des.door({state="closed",coord={49,13}})
des.door({state="closed",coord={44,13}})
des.door({state="closed",coord={31,13}})
des.door({state="closed",coord={26,13}})
des.door({state="locked",coord={20,13}})
des.door({state="closed",coord={49,11}})
des.door({state="locked",coord={20,10}})
des.door({state="closed",coord={49,9}})
des.door({state="locked",coord={55,7}})
des.door({state="closed",coord={49,7}})
des.door({state="closed",coord={44,7}})
des.door({state="closed",coord={31,7}})
des.door({state="closed",coord={26,7}})
des.door({state="locked",coord={20,7}})
des.trap({type="land mine",coord={14,10}})
des.trap({type="land mine",coord={16,10}})
des.monster({class="F",id="shrieker",coord={11,14}})
des.monster({class="F",id="shrieker",coord={5,12}})
des.monster({class="F",id="shrieker",coord={11,5}})
des.monster({class="F",id="shrieker",coord={4,8}})
des.monster({class="F",id="shrieker",coord={70,15}})
des.monster({class="F",id="shrieker",coord={65,10}})
des.monster({class="F",id="shrieker",coord={67,6}})
des.monster({class="F",id="shrieker",coord={63,16}})
des.region({region={16,6,19,8},lit=1,type="barracks",filled=1})
des.region({region={16,12,19,14},lit=1,type="barracks",filled=1})
des.region({region={21,9,25,11},lit=1,type="barracks",filled=1})
des.region({region={27,7,30,9},lit=1,type="barracks",filled=1})
des.region({region={27,11,30,13},lit=1,type="barracks",filled=1})
des.region({region={56,7,59,9},lit=1,type="barracks",filled=1})
des.region({region={56,11,59,13},lit=1,type="barracks",filled=1})
des.region({region={56,6,59,8},lit=1,type="barracks",filled=1})
des.region({region={56,12,59,14},lit=1,type="barracks",filled=1})
des.region({region={32,9,35,11},lit=1,type="weapon shop",filled=1})
des.region({region={40,9,43,11},lit=1,type="wand shop",filled=1})
des.region({region={50,9,54,11},lit=1,type="throne"})
des.monster({class="h",id="master mind flayer",coord={54,10},asleep=true})
des.object({class="/",coord={54,10}})
des.object({class="!",coord={54,10}})
des.object({class="!",coord={54,10}})
des.object({class="!",coord={54,10}})
des.object({class="?",coord={54,10}})
des.object({class="?",coord={54,10}})
des.monster({class="h",id="deep one",coord={50,9},asleep=true})
des.monster({class="h",id="deep one",coord={50,10},asleep=true})
des.monster({class="h",id="deep one",coord={50,11},asleep=true})
des.monster({class="h",id="deep one",coord={51,9},asleep=true})
des.monster({class="h",id="deeper one",coord={51,10},asleep=true})
des.monster({class="h",id="deep one",coord={51,11},asleep=true})
des.monster({class="h",id="deep one",coord={52,9},asleep=true})
des.monster({class="h",id="deep one",coord={52,10},asleep=true})
des.monster({class="h",id="deep one",coord={52,11},asleep=true})
