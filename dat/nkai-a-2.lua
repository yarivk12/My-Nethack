-- Step 10C-A fixed map resource: nkai-a-2
-- Pinned donor: neutrality.des at commit 17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0
-- Static body port only; topology, scheduler, and generator hooks remain deferred.
des.map({halign="center",valign="center",map=[=[
                                 }}}}                                       
 ...    .....            ...    }}..}}  ....        .....       .....   ... 
 ...  ....  ..     ... ... ....}.}..}.}   ...    ..........   ...   .....   
  .  ...     ...  .. ...    ..}.}....}.}    . .......    .......    ..  ..  
  ....         .....   .... .}.}.....}..}   ....     ...      .......  ..   
    ...   .... .. ..    ..  }..}....}...}. ...     ...... ....      ....    
   .. ..... ....   ..  ..  }....}..}...}.....    .....  ...   ...   . ...   
  ..    ..   ..     ....  }.}..}.}..}.}..      ....  ........  .... .   ..  
 ..    ....   ...  .. ... .}....}}}..}}}...  ...     ...........  ...    .  
  ..   .. ..    ....   ....}...}.}}}}}.}} ...  .... ........  ..   ..    .  
  ... ..   ...  .. ...   ...}.}...}}}...}  ..    ....  ...     .. ...    .. 
    ...      ....    ...  ..}..}...}}}...}   ...   ..    ...    ....    ... 
   .....    ..  ...    ....  }}...}}}...}..    ..   ..     ..  ..  ..   ..  
  ... ...    ..  ..     ..   .}....}}}...}..   ...   .... ... ... ..    .   
 ...    ...   ....  ...  ......}....}}}.}. .. .. ...   .... ... ...   ...   
 ...     ... ... ..   .....  ...}....}}}.   ...   .....      ..    ....     
  ...     ....     ..  ...    ...}..}}}.     .    .   .....      .... ..    
   ....  ....  ...  ....    ...   }..}}}... .... ... ..   ........     ...  
  ... ....  .... .....    ....     }}}.}  ... .... ...    .   ......    ... 
                                   }}}}                                     
]=]})
des.region({region={1,1,75,19},lit=0,type="morgue",filled=0})
des.stair({dir="up"})
des.trap({type="hole"})
des.trap({type="hole"})
des.trap({type="hole"})
des.trap({type="hole"})
des.levregion({region={1,1,74,18},exclude={45,4,68,15},type="branch"})
des.monster({class="h",id="deeper one",coord={33,2}})
des.monster({class=";",coord={36,8}})
des.monster({class=";",coord={39,7}})
des.monster({class=";",coord={37,12}})
des.monster({class=";",coord={31,14}})
des.monster({class=";",coord={37,17}})
des.monster({class="g"})
des.monster({class="g"})
des.monster({class="g"})
des.monster({class="g"})
des.monster({class="B"})
des.monster({class="B"})
des.monster({class="B"})
des.monster({class="B"})
des.monster({class="B"})
des.monster({class="B"})
des.monster({class="B"})
des.monster({class="B"})
des.monster({class="g",id="nightgaunt"})
des.monster({class="g",id="nightgaunt"})
des.monster({class="g",id="nightgaunt"})
des.monster({class="B",id="byakhee"})
des.monster({class="B",id="byakhee"})
des.monster({class="B",id="byakhee"})
des.monster({class="B",id="byakhee"})
des.monster({class="B",id="byakhee"})
des.monster({class="Y",id="gug"})
des.monster({class="Y",id="gug"})
des.monster({class="Y",id="gug"})
des.monster({class="Y",id="gug"})
des.monster({class="Y",id="gug"})
des.monster({class="Z"})
des.monster({class="Z"})
des.monster({class="Z"})
des.monster({class="W"})
des.monster({class="V"})
des.monster({class="P"})
des.monster({class="P"})
des.monster({class="b",id="shoggoth"})
des.object({class="?"})
des.object({class="?"})
des.object({class="+"})
des.object({class="/"})
des.object({class="("})
des.object({class="("})
des.object({class=")"})
des.object({class=")"})
des.object({class="["})
des.object({class="["})
des.object({class="["})
des.object({class="*"})
des.object({class="*"})
des.object({class="*"})
des.object({class="*"})
des.trap({type="spiked pit"})
des.trap({type="spiked pit"})
des.trap({type="spiked pit"})
des.trap({type="magic"})
des.trap({type="magic"})
des.trap({type="magic"})
des.trap({type="magic"})
des.trap({type="anti magic"})
des.trap({type="anti magic"})
des.trap({type="anti magic"})
des.trap({type="pit"})
des.trap({type="pit"})
des.trap({type="pit"})
des.trap({type="board"})
des.trap({type="board"})
des.trap({type="board"})
if percent(75) then
des.monster({class="X",id="bestial dervish"})
end
if percent(75) then
des.monster({class="X",id="ethereal dervish"})
end
if percent(75) then
des.monster({class="P",id="sparkling lake"})
end
if percent(75) then
des.monster({class="P",id="flashing lake"})
end
if percent(75) then
des.monster({class="P",id="smoldering lake"})
end
if percent(75) then
des.monster({class="P",id="frosted lake"})
end
if percent(75) then
des.monster({class="v",id="blood shower"})
end
if percent(75) then
des.monster({class="U",id="many-taloned thing"})
end
if percent(75) then
des.monster({class="b",id="deep blue cube"})
end
if percent(75) then
des.monster({class="b",id="pitch black cube"})
end
if percent(75) then
des.monster({class="U",id="prayerful thing"})
end
if percent(75) then
des.monster({class="U",id="hemorrhagic thing"})
end
if percent(75) then
des.monster({class="e",id="many-eyed seeker"})
end
if percent(75) then
des.monster({class=" ",id="voice in the dark"})
end
if percent(75) then
des.monster({class="y",id="tiny being of light"})
end
if percent(75) then
des.monster({class="s",id="man-faced millipede"})
end
if percent(75) then
-- Donor class '{' is not a local monster class; identity remains name-resolved.
des.monster({id="mirrored moonflower"})
end
if percent(75) then
des.monster({class="w",id="crimson writher"})
end
if percent(75) then
des.monster({class="p",id="radiant pyramid"})
end
