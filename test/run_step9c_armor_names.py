# My-NetHack modification notice, added 2026-10-15.
# Local project integration/modifications; original authorship is retained.
# Recorded project change dates (Git author dates): 2026-09-09.
# See doc/license-compliance.md and doc/my-nethack-modifications.json.
# Existing copyright, attribution, license and warranty notices still apply.
"""Actual object parser: symbiote names/descriptions and native armor aliases."""
import sys
from run_step8a_runtime import Game
release,output=sys.argv[1:3]
game=Game(release,output)
try:
    game.lua('''
for _,pair in ipairs({
 {"living armor","living armor"},
 {"barnacle armor","barnacle armor"},
 {"giant sea anemone","living armor"},
 {"giant shell armor","barnacle armor"},
 {"leather armor","leather armor"},
 {"studded leather armor","studded leather armor"},
 {"plate armor","plate mail"},
 {"ring mail","ring mail"}
}) do
 for _,prefix in ipairs({"","uncursed +0 ","blessed +3 "}) do
  local o=obj.new(prefix..pair[1]);local t=o:totable()
  assert(t.otyp_name==pair[2],pair[1].." became "..t.otyp_name)
 end
end
''')
    print('PASS 24 actual symbiote name/description and native armor alias parses',flush=True)
finally:
    game.close()
