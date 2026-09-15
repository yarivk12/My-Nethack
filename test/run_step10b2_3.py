"""Compile the Step 10B2-3 declaration/helper gate outside the repository."""
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
    "src/mondata.c": ("step10b_innate_magic", "step10b_eldritch_presence_kind"),
    "src/mcastu.c": ("step10b_spell_cooldown", "step10b_species_spell"),
    "src/mhitu.c": ("step10b_illurien_forget_percent",),
    "src/questpgr.c": ("step10b_neutral_montype", "step10b_sum_montype"),
}.items():
    source = (repo / path).read_text(encoding="utf8")
    for name in names:
        parts.append(re.search(r"(?m)^(?:boolean|int)\n" + name
                               + r"\([\s\S]*?^\}", source)[0])
(out / "step10b2_3_helpers.h").write_text("\n".join(parts), encoding="utf8")

test = (repo / "test/test_step10b2_3.c").read_text(encoding="utf8")
test = test.replace("#undef MCASTU_ENUM", "#undef MCASTU_ENUM\n"
                    "#include \"step10b2_3_helpers.h\"")
(out / "test_step10b2_3.c").write_text(test, encoding="utf8")
subprocess.run([
    "cl", "/nologo", "/std:c11", "/W4", "/WX",
    "/D_CRT_SECURE_NO_WARNINGS", "/DWIN32", "/DWIN32CON",
    "/I" + str(repo / "include"), "/I" + str(repo / "submodules/lua"),
    "/I" + str(out), "/Fe:" + str(out / "step10b2_3.exe"),
    str(out / "test_step10b2_3.c"), str(repo / "src/monst.c"),
    str(repo / "src/objects.c")
], cwd=out, check=True)
result = subprocess.check_output([str(out / "step10b2_3.exe")], cwd=out,
                                 text=True)
assert "PASS Step 10B2-3" in result
print(result, end="")

quest = (repo / "src/questpgr.c").read_text(encoding="utf8")
rlyeh = re.search(r"(?m)^staticfn int\nstep10b_rlyeh_emit\([\s\S]*?"
                   r"^\}\n\nint\nstep10b_rlyeh_create\([\s\S]*?^\}", quest)[0]
(out / "step10b2_3_rlyeh.h").write_text(rlyeh, encoding="utf8")
rtest = (repo / "test/test_step10b2_3_rlyeh.c").read_text(encoding="utf8")
rtest = rtest.replace("struct instance_globals_saved_m svm;",
                      "struct instance_globals_saved_m svm;\n"
                      "#include \"step10b2_3_rlyeh.h\"")
(out / "test_step10b2_3_rlyeh.c").write_text(rtest, encoding="utf8")
subprocess.run([
    "cl", "/nologo", "/std:c11", "/W4", "/WX",
    "/D_CRT_SECURE_NO_WARNINGS", "/DWIN32", "/DWIN32CON",
    "/I" + str(repo / "include"), "/I" + str(repo / "submodules/lua"),
    "/I" + str(out), "/Fe:" + str(out / "step10b2_3_rlyeh.exe"),
    str(out / "test_step10b2_3_rlyeh.c"), str(repo / "src/monst.c")
], cwd=out, check=True)
result = subprocess.check_output([str(out / "step10b2_3_rlyeh.exe")], cwd=out,
                                 text=True)
assert "PASS Step 10B2-3 R'lyeh" in result
print(result, end="")
