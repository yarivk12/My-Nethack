-- NetHack special level; see doc/step9a.md.
-- UnNetHack 439b8d63d3d1ca78fb08588dd43f61874114b21a:dat/sheol.des
des.level_init({style="solidfill", fg=" "})
des.level_flags("hardfloor", "noteleport", "noflip")
des.map({halign="center", valign="center", map=[=[
      |..................-----------   -----------...........|              
      |.......-.....-....--........-----........--.....-.....|              
      |......| |...| |...Y+........|.\.|........+Y....| |....|              
      |...UU..-.....-....-----------...-----------.....-.....|-----------   
      |...U..............+.......YYY...YYY.......+...........S..........|   
      |...U..............+.......YYY...YYY.......+...........|---------.|   
      |...UU..-.....-...------------...------------....-.....|        |.|   
      |......| |...| |..|          |...|          |...| |....|---     |.|   
      |.......-.....-...---------- |...| ----------....-....---P|     |.|   
      |..................--......---+++---......IP--.......--PPP|     |.|   
      --------------------...---...........---..IPP---...---PPPP|     |.|   
     --PPPIIIIIIIIIIIIIIII...| |.---.{.---.| |..IIPPP-----PPPPPP|     |.|   
    --PPPPPIIIIIIIIIIIIIIII..---.| |...| |.---...IIPPPPPPIIIPIII|     |.|   
   --PPPPPIIIIIIIIIIIIIIIII......---.{.---........IIPPPIII.III..-------+----
  --PPPPPPIIIIIIIIIIIIIIII.........................IIIII........UUYY..III...
 -------------IIIIIIIIIII............{.....................T..T.UUYY........
             -------IIIII.......................................UUYY........
                   ---IIII.....----.....--------IIII............UUYY........
                     -----------  ---.---      ----IIIII...T..T.UUYY........
                                    ---           ----III.......UUYY........
]=]})
des.stair("up",37,18)
des.levregion({region={7,2,60,7}, exclude={10,2,56,7}, type="stair-down"})
des.teleport_region({region={0,14,63,18},exclude={0,0,0,0}})
des.monster({id="crystal ice golem",coord={34,2}})
des.monster({id="crystal ice golem",coord={40,2}})
des.monster({id="blue slime"})
des.monster({id="blue slime"})
des.monster({id="blue slime"})
des.monster({id="chillbug",coord={16, 4}})
des.monster({id="chillbug",coord={17, 4}})
des.monster({id="chillbug",coord={16, 5}})
des.monster({id="chillbug",coord={17, 5}})
des.monster({id="chillbug",coord={58, 4}})
des.monster({id="chillbug",coord={57, 4}})
des.monster({id="chillbug",coord={58, 5}})
des.monster({id="chillbug",coord={57, 5}})
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
des.monster()
des.monster()
des.monster()
des.non_diggable(selection.area(0,0,75,19))
des.door("locked",36,9)
des.door("locked",37,9)
des.door("locked",38,9)
des.door("locked",26,2)
des.door("locked",25,4)
des.door("locked",25,5)
des.door("locked",48,2)
des.door("locked",49,4)
des.door("locked",49,5)
des.door("locked",71,13)
des.trap()
des.trap()
des.trap()
des.trap()
des.trap()
des.trap()
des.region({region={28,9,46,18},lit=1,type="ordinary"})
des.object({id="chest",coord={34,2}})
des.object({id="chest",coord={40,2}})
des.object({class="?",coord={34,1}})
des.object({class="?",coord={40,1}})
des.object({class="?",coord={33,1}})
des.object({class="?",coord={33,2}})
des.object({class="?",coord={41,1}})
des.object({class="?",coord={41,2}})
des.object({class="!",coord={32,1}})
des.object({class="!",coord={32,2}})
des.object({class="!",coord={31,1}})
des.object({class="!",coord={31,2}})
des.object({class="!",coord={30,1}})
des.object({class="!",coord={30,2}})
des.object({class="!",coord={29,1}})
des.object({class="!",coord={29,2}})
des.object({class="!",coord={28,1}})
des.object({class="!",coord={28,2}})
des.object({class="\"",coord={27,1}})
des.object({class="\"",coord={27,2}})
des.object({class="?",coord={42,1}})
des.object({class="?",coord={42,2}})
des.object({class="?",coord={43,1}})
des.object({class="?",coord={43,2}})
des.object({class="?",coord={44,1}})
des.object({class="?",coord={44,2}})
des.object({class="?",coord={45,1}})
des.object({class="?",coord={45,2}})
des.object({class="!",coord={46,1}})
des.object({class="!",coord={46,2}})
des.object({class="!",coord={47,1}})
des.object({class="!",coord={47,2}})
des.object({class="+"})
des.object({class="+"})
des.object({class="="})
des.object({class="="})
des.object({class="/"})
des.object({class="/"})
