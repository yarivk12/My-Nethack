"""Compile the Step 10B2-4 declaration/helper gate outside the repository."""
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
    "src/worn.c": ("step10b_natural_dr", "mith_physical_damage"),
    "src/mondata.c": ("step10b_innate_magic", "step10b_eldritch_presence_kind"),
    "src/muse.c": ("step10b_innate_reflection",),
    "src/mcastu.c": ("step10b_spell_cooldown", "step10b_species_spell"),
    "src/questpgr.c": ("step10b_center_candidate", "step10b_alhoon_key_choice"),
    "src/makemon.c": ("step10b_center_peaceful", "step10b_is_witch",
                       "step10b_link_witch_familiar",
                       "step10b_witch_needs_familiar"),
    "src/weapon.c": ("mith_offhand_attack",),
    "src/monmove.c": ("step10b_cthulhu_psychic_damage",),
    "src/mhitu.c": ("step10b_wisdom_drain_amount",),
}.items():
    source = (repo / path).read_text(encoding="utf8")
    for name in names:
        parts.append(re.search(r"(?m)^(?:boolean|int|void)\n" + name
                               + r"\([\s\S]*?^\}", source)[0])
(out / "step10b2_4_helpers.h").write_text("\n".join(parts), encoding="utf8")

test = (repo / "test/test_step10b2_4.c").read_text(encoding="utf8")
test = test.replace("#undef MCASTU_ENUM", "#undef MCASTU_ENUM\n"
                    "#include \"step10b2_4_helpers.h\"")
(out / "test_step10b2_4.c").write_text(test, encoding="utf8")
subprocess.run([
    "cl", "/nologo", "/std:c11", "/W4", "/WX",
    "/D_CRT_SECURE_NO_WARNINGS", "/DWIN32", "/DWIN32CON",
    "/I" + str(repo / "include"), "/I" + str(repo / "submodules/lua"),
    "/I" + str(out), "/Fe:" + str(out / "step10b2_4.exe"),
    str(out / "test_step10b2_4.c"), str(repo / "src/monst.c"),
    str(repo / "src/objects.c")
], cwd=out, check=True)
result = subprocess.check_output([str(out / "step10b2_4.exe")], cwd=out,
                                 text=True)
assert "PASS Step 10B2-4" in result
print(result, end="")
