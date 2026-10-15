# My-NetHack modification notice, added 2026-10-15.
# Local project integration/modifications; original authorship is retained.
# Recorded project change dates (Git author dates): 2026-09-24.
# See doc/license-compliance.md and doc/my-nethack-modifications.json.
# Existing copyright, attribution, license and warranty notices still apply.
"""Source guard for native #overview forge coverage."""
from pathlib import Path

R = Path(__file__).resolve().parents[1]

dungeon_h = (R / "include/dungeon.h").read_text()
dungeon_c = (R / "src/dungeon.c").read_text()

assert "Bitfield(forge, 1)" in dungeon_h
assert "Bitfield(spare1, 1)" not in dungeon_h
assert "case FORGE:" in dungeon_c
assert "mptr->flags.forge = 0" in dungeon_c
assert "|| mptr->flags.forge" in dungeon_c
assert 'ADDTOBUF("forge", mptr->flags.forge)' in dungeon_c

# The existing serialized flag slot is reused. No new mapseen field or codec
# branch is allowed to turn this display-only presence bit into a format edit.
mapseen_codec = (R / "src/dungeon.c").read_text()
assert "Sfo_mapseen_flags" in mapseen_codec
assert "Sfi_mapseen_flags" in mapseen_codec
print("PASS #overview forge source wiring and unchanged mapseen codec boundary")
