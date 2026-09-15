"""Compile the Step 10B3-1 object/helper gate outside the repository."""
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
    "src/o_init.c": ("step10b_extension_otyp", "step10b_oclass_first",
                     "step10b_oclass_next"),
    "src/potion.c": ("step10b_amnesia_percent",),
}.items():
    source = (repo / path).read_text(encoding="utf8")
    for name in names:
        parts.append(re.search(r"(?m)^(?:boolean|int)\n" + name
                               + r"\([\s\S]*?^\}", source)[0])
(out / "step10b3_1_helpers.h").write_text("\n".join(parts), encoding="utf8")

test = (repo / "test/test_step10b3_1.c").read_text(encoding="utf8")
test = test.replace("int\nmain(void)",
                    '#include "step10b3_1_helpers.h"\n\nint\nmain(void)')
(out / "test_step10b3_1.c").write_text(test, encoding="utf8")
subprocess.run([
    "cl", "/nologo", "/std:c11", "/W4", "/WX",
    "/D_CRT_SECURE_NO_WARNINGS", "/DWIN32", "/DWIN32CON",
    "/I" + str(repo / "include"), "/I" + str(repo / "submodules/lua"),
    "/I" + str(out), "/Fe:" + str(out / "step10b3_1.exe"),
    str(out / "test_step10b3_1.c"), str(repo / "src/objects.c")
], cwd=out, check=True)
result = subprocess.check_output([str(out / "step10b3_1.exe")], cwd=out,
                                 text=True)
assert "PASS Step 10B3-1" in result
print(result, end="")
