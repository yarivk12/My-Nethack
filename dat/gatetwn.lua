-- Step 10C-A fixed map resource: gatetwn
-- Pinned donor: neutrality.des at commit 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0

des.level_flags("hardfloor")
des.map({halign="center",valign="center",map=[=[
0      }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}         
1  }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}   
2 }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}} 
3 }}}}......T.......................................................T.....}}}
4 }}}}..T.w....PPPPPPPPPPPPPPPPPPPPP--++--PPPPPPPPPPPPPPPPPPPPP...........}}}
5}}}...........P--F--F..............-....-..............F--F--P...T..w.....}}
6}}}.T.........PF....--+---F----------..--------------+--....FP.......T....}}
7}}}....w..T...P-....+..-.....-................-.....-..+....-P.T...T......}}
8}}............P-....-..-.....-.--------------.-.....-..F....-P..........T..}
9}}.....T.w....P-FFFF-..--F+F--.-.....FF.....-.---+F--..-----FP..T...T.......
0}}..................+..........+.....FF.....+..........+........w.....T.....
1}}...T........P-FFFF-..--F+---.-.....FF.....-.---+---..-----FP.....T.....T. 
2}........T....P-....-..-.....-.--------------.|.....-..F....-P.....w....... 
3}.............P-....+..-.....-................|.....-..+....-P..T....T...   
4}..T..T.......PF....--+--------------..--------------+--....FP...........T  
5}.......w.....P--F--F..............-....-..............F--F--P.w..T.....   T
6.T.......T....PPPPPPPPPPPPPPPPPPPPP--++--PPPPPPPPPPPPPPPPPPPPP.........     
7......w.....T.............T...............T...................T.....T.      
8...T...............T...................T...........T...............         
9....                                                                        
0.                                                                           
]=]})
des.levregion({region={3,3,72,17},exclude={14,4,61,16},type="branch"})
-- Portal contract: region={0,20,0,20}, exclude={0,0,0,0}, destination="out1".
des.levregion({region={0,20,0,20},exclude={0,0,0,0},type="portal",name="out1"})
des.door({state="closed",coord={20,7}})
des.door({state="closed",coord={20,10}})
des.door({state="closed",coord={20,13}})
des.door({state="closed",coord={26,9}})
des.door({state="closed",coord={26,11}})
des.door({state="closed",coord={31,10}})
des.door({state="closed",coord={37,4}})
des.door({state="closed",coord={38,4}})
des.door({state="closed",coord={37,16}})
des.door({state="closed",coord={38,16}})
des.door({state="closed",coord={44,10}})
des.door({state="closed",coord={49,9}})
des.door({state="closed",coord={49,11}})
des.door({state="closed",coord={55,7}})
des.door({state="closed",coord={55,10}})
des.door({state="closed",coord={55,13}})
des.region({region={0,0,75,20},lit=1,type="ordinary"})
des.region({region={32,9,36,11},lit=1,type="armor shop",filled=1})
des.region({region={39,9,43,11},lit=1,type="potion shop",filled=1})
des.region({region={16,12,19,14},lit=1,type="food shop",filled=1})
des.region({region={16,6,19,8},lit=1,type="food shop",filled=1})
des.region({region={56,12,59,14},lit=1,type="food shop",filled=1})
des.region({region={56,6,59,8},lit=1,type="food shop",filled=1})
des.region({region={24,7,28,8},lit=1,type="temple"})
des.altar({coord={26,7},align="noncoaligned",type="shrine"})
des.region({region={47,7,51,8},lit=1,type="tool shop",filled=1})
des.region({region={24,12,28,13},lit=1,type="shop",filled=1})
des.region({region={47,12,51,13},lit=0,type="beehive",filled=1})
