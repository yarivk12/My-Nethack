"""Run from a VS developer shell: python test/run_step7.py OUTPUT_DIRECTORY.

Compiles current production scheduler/light bodies with fixture adapters and
the actual monster/object databases. Outputs stay outside the source tree.
"""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
out = Path(sys.argv[1]).resolve()
out.mkdir(parents=True, exist_ok=True)
parts = re.findall(r"(?m)^#define STEP6B_(?:MIN|MAX)_LEVEL[^\n]*",
                   (repo / "src/dungeon.c").read_text())
assert len(parts) == 2
for path, names in {
    "src/dungeon.c": ["depth", "step6b_depth_used", "step6b_pick_depth",
                      "step6b_add_level", "step6b_schedule"],
    "src/makemon.c": ["uncommon"],
    "src/light.c": ["candle_light_range"],
    "src/timeout.c": ["begin_burn", "end_burn"],
    "src/shk.c": ["cost_per_charge"],
}.items():
    source = (repo / path).read_text(encoding="utf-8")
    for name in names:
        matches = list(re.finditer(r"(?m)^(?:staticfn )?\w+\n" + name
                                  + r"\([\s\S]*?^\}", source))
        assert len(matches) == 1, (path, name)
        parts.append(matches[0][0])
(out / "step7_functions.h").write_text("\n\n".join(parts), encoding="utf-8")
cmd = ["cl", "/nologo", "/std:c11", "/W4", "/D_CRT_SECURE_NO_WARNINGS",
       "/DWIN32", "/DWIN32CON", "/I" + str(repo / "include"),
       "/I" + str(repo / "submodules/lua"), "/I" + str(out),
       "/Fe:" + str(out / "step7_runtime.exe"),
       str(repo / "test/test_step7_runtime.c"), str(repo / "src/monst.c"),
       str(repo / "src/objects.c")]
subprocess.run(cmd, cwd=out, check=True)
subprocess.run([str(out / "step7_runtime.exe")], cwd=out, check=True)
