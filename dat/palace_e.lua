-- NetHack special level; see doc/step9a.md.
-- UnNetHack 439b8d63d3d1ca78fb08588dd43f61874114b21a:dat/sheol.des
des.level_init({style="solidfill", fg=" "})
des.level_flags("hardfloor", "noteleport", "noflip")
des.map({halign="center", valign="center", map=[=[
                                                                            
                                                                            
         --------------------------------------------------                 
        --..........................................YYYYYY-----             
        |.....--........--......................YYYYY....YYYYY----          
        |....|  |......|  |.....-............YYYY............YYYY---        
      ---....|  |......|  |....| |..--.T...YYY.....T...T...T....YYY---      
      |.......--...T....--...T..-...--...YYY......................YYY--     
   ----.................................YY..........................YY--    
   |...............{.........{.........YY....................\.......YY-    
   ----.................................YY..........................YY--    
      |.......--...T....--...T..-...--...YYY......................YYY--     
      ---....|  |......|  |....| |..--.T...YYY.....T...T...T....YYY---      
        |....|  |......|  |.....-............YYYY............YYYY---        
        |.....--........--......................YYYYY....YYYYY----          
        --..........................................YYYYYY-----             
         --------------------------------------------------                 
                                                                            
                                                                            
                                                                            
]=]})
des.stair("up",4,9)
des.non_diggable(selection.area(0,0,75,19))
des.monster({id="Executioner",coord={61,9}})
des.trap()
des.trap()
des.trap()
des.trap()
des.trap()
des.trap()
des.trap()
des.trap()
if percent(10) then des.grave({text="Isaac - Sacrificed by Mother"}) end
if percent(30) then des.grave({text="Guppy - Beloved by Flies"}) end
des.object({id="chest",coord={62,8}})
des.object({id="chest",coord={62,10}})
des.object({id="chest",coord={64,8}})
des.object({id="chest",coord={64,10}})
des.object({id="chest",coord={66,8},contents=function()
des.object({id="crystal pick"})
end})
des.object({id="chest",coord={66,10},contents=function()
des.object({id="crystal pick"})
end})
des.object({id="chest",coord={68,9},contents=function()
des.object({id="magic marker"})
end})
des.monster({id="blue slime"})
des.monster({id="blue slime"})
des.monster({id="blue slime"})
des.monster({id="blue slime"})
des.monster({id="blue slime"})
des.monster({id="blue slime"})
des.monster({id="blue slime"})
des.monster({id="blue slime"})
des.monster({id="white naga"})
des.monster({id="white naga"})
des.monster({id="white naga"})
des.monster({id="white naga"})
des.monster({id="crystal ice golem"})
des.monster({id="crystal ice golem"})
des.monster({id="ice golem"})
des.monster({id="ice golem"})
des.monster({id="dark Angel"})
des.monster({id="dark Angel"})
des.monster({id="dark Angel"})
des.monster()
des.monster()
des.monster()
des.monster()
des.monster()
des.monster()
des.monster()
des.monster()
des.monster()
des.monster()
des.monster()
