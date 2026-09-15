"""Extract and run dispatcher-sensitive Step 10B2-4 production helpers."""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
artifact_root = repo / "_qa"
out = Path(sys.argv[1]).resolve()
assert out != repo and (out == artifact_root or artifact_root in out.parents)
out.mkdir(parents=True, exist_ok=True)

specs = [
    ("src/mon.c", "void", "step10b_cthulhu_death_effect"),
    ("src/mon.c", "staticfn void", "mith_deep_soul"),
    ("src/mon.c", "void", "step10b_familiar_died"),
    ("src/mthrowu.c", "int", "mith_internal_projectile"),
    ("src/makemon.c", "boolean", "step10b_is_witch"),
    ("src/makemon.c", "void", "step10b_link_witch_familiar"),
    ("src/makemon.c", "boolean", "step10b_witch_needs_familiar"),
    ("src/makemon.c", "struct monst *", "step10b_create_witch_familiar"),
]
parts = []
for path, rettype, name in specs:
    source = (repo / path).read_text(encoding="utf8")
    parts.append(re.search(r"(?m)^" + re.escape(rettype) + r"\n" + name
                           + r"\([\s\S]*?^\}", source)[0])
test = (repo / "test/test_step10b2_4_runtime.c").read_text(encoding="utf8")
test = test.replace("/* EXTRACTED */", "\n\n".join(parts))
generated = out / "test_step10b2_4_runtime.c"
generated.write_text(test, encoding="utf8")
subprocess.run([
    "cl", "/nologo", "/std:c11", "/W4", "/WX",
    "/D_CRT_SECURE_NO_WARNINGS", "/DWIN32", "/DWIN32CON",
    "/I" + str(repo / "include"), "/I" + str(repo / "submodules/lua"),
    "/Fe:" + str(out / "step10b2_4_runtime.exe"), str(generated),
    str(repo / "src/monst.c")
], cwd=out, check=True)
result = subprocess.check_output([str(out / "step10b2_4_runtime.exe")],
                                 cwd=out, text=True)
assert "PASS Step 10B2-4 runtime" in result
print(result, end="")
