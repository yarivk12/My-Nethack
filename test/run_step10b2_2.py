"""Compile the Step 10B2-2 focused declaration/mechanics gate externally."""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
artifact_root = repo / "_qa"
out = Path(sys.argv[1]).resolve()
assert out != repo and (out == artifact_root or artifact_root in out.parents)
out.mkdir(parents=True, exist_ok=True)

parts = []
for path, names in {
    "src/worn.c": ("step10b_natural_dr",),
    "src/mondata.c": ("step10b_innate_magic",
                      "step10b_eldritch_presence_kind",
                      "step10b_mark_eldritch_seen"),
    "src/muse.c": ("step10b_innate_reflection",),
    "src/uhitm.c": ("step10b_passive_dice",
                     "step10b_elemental_passive_ready"),
    "src/mcastu.c": ("step10b_spell_cooldown", "step10b_species_spell"),
    "src/weapon.c": ("mith_multiweapon_slot", "mith_bimanual",
                     "mith_select_multiweapon"),
}.items():
    source = (repo / path).read_text(encoding="utf8")
    for name in names:
        parts.append(re.search(r"(?m)^(?:boolean|int)\n" + name
                               + r"\([\s\S]*?^\}", source)[0]
                     if name != "mith_select_multiweapon"
                     else re.search(r"(?m)^struct obj \*\n" + name
                                    + r"\([\s\S]*?^\}", source)[0])
(out / "step10b2_2_hwep.h").write_text(
    re.search(r"(?m)^static const NEARDATA short hwep\[\] = \{[\s\S]*?^\};",
              (repo / "src/weapon.c").read_text(encoding="utf8"))[0],
    encoding="utf8")
(out / "step10b2_2_helpers.h").write_text("\n".join(parts), encoding="utf8")

test = (repo / "test/test_step10b2_2.c").read_text(encoding="utf8")
test = test.replace('#undef MCASTU_ENUM',
                    '#undef MCASTU_ENUM\n#include "step10b2_2_hwep.h"\n'
                    '#include "step10b2_2_helpers.h"')
(out / "test_step10b2_2.c").write_text(test, encoding="utf8")
subprocess.run([
    "cl", "/nologo", "/std:c11", "/W4", "/WX",
    "/D_CRT_SECURE_NO_WARNINGS", "/DWIN32", "/DWIN32CON",
    "/I" + str(repo / "include"), "/I" + str(repo / "submodules/lua"),
    "/I" + str(out), "/Fe:" + str(out / "step10b2_2.exe"),
    str(out / "test_step10b2_2.c"),
    str(repo / "src/monst.c"), str(repo / "src/objects.c")
], cwd=out, check=True)
result = subprocess.check_output([str(out / "step10b2_2.exe")], cwd=out,
                                 text=True)
assert "PASS Step 10B2-2" in result
print(result, end="")
