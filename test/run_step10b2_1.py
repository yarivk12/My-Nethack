"""Compile the Step 10B2-1 declaration gate under the project _qa directory."""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
artifact_root = repo / "_qa"
out = Path(sys.argv[1]).resolve()
assert out != repo and (out == artifact_root or artifact_root in out.parents)
out.mkdir(parents=True, exist_ok=True)
quest = (repo / "src/questpgr.c").read_text(encoding="utf8")
parts = []
for name in ("step10b_neutral_montype", "step10b_sum_montype",
             "step10b_neutral_squad"):
    parts.append(re.search(r"(?m)^int\n" + name + r"\([\s\S]*?^\}", quest)[0])
for path, names in {
    "src/worn.c": ("step10b_natural_dr",),
    "src/muse.c": ("step10b_innate_reflection",),
    "src/mondata.c": ("step10b_innate_magic",),
    "src/mcastu.c": ("step10b_spell_cooldown", "step10b_species_spell"),
    "src/uhitm.c": ("step10b_backstab_die",),
    "src/weapon.c": ("mith_offhand_attack",),
}.items():
    source = (repo / path).read_text(encoding="utf8")
    for name in names:
        parts.append(re.search(r"(?m)^(?:boolean|int)\n" + name
                               + r"\([\s\S]*?^\}", source)[0])
(out / "step10b2_1_selectors.h").write_text("\n".join(parts), encoding="utf8")
subprocess.run([
    "cl", "/nologo", "/std:c11", "/W4", "/WX",
    "/D_CRT_SECURE_NO_WARNINGS", "/DWIN32", "/DWIN32CON",
    "/I" + str(repo / "include"), "/I" + str(repo / "submodules/lua"),
    "/I" + str(out),
    "/Fe:" + str(out / "step10b2_1.exe"),
    str(repo / "test/test_step10b2_1.c"),
    str(repo / "src/monst.c"), str(repo / "src/objects.c")
], cwd=out, check=True)
result = subprocess.check_output([str(out / "step10b2_1.exe")], cwd=out, text=True)
assert "PASS Step 10B2-1" in result
print(result, end="")
