"""Build and run the exact 1,000-seed extracted-production scheduler gate."""
from collections import Counter
from pathlib import Path
import json
import re
import subprocess
import sys


repo = Path(__file__).resolve().parents[1]
artifact_root = repo / "_qa"
out = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else Path(
    artifact_root / "step10b-tests" / "x64" / "step10c-b-1000")
assert out != repo and (out == artifact_root or artifact_root in out.parents)
out.mkdir(parents=True, exist_ok=True)
source = (repo / "src/dungeon.c").read_text(encoding="utf-8")
parts = re.findall(
    r"(?m)^#define (?:STEP6B_(?:MIN|MAX)_LEVEL|STEP10C_DISPENSARY_LEVEL)[^\n]*",
    source,
)
parts.extend([
    "staticfn boolean step6b_scheduled_branch(const branch *);",
    "staticfn boolean step6b_scheduled_approach(const s_level *);",
    "staticfn boolean step10c_internal_depth(d_level *, int *);",
])
for name in (
    "depth", "step6b_depth_used", "step6b_pick_depth", "step6b_add_level",
    "step6b_scheduled_branch", "step6b_scheduled_approach",
    "step6b_rebase_level", "step6b_rebase_branch",
    "step10c_internal_branch", "step10c_select_alternates",
    "step10c_internal_depth", "step6b_schedule",
):
    matches = list(re.finditer(
        r"(?m)^(?:staticfn )?(?:\w+\s*\*)?\w*\n" + name
        + r"\([\s\S]*?^\}", source
    ))
    assert len(matches) == 1, name
    parts.append(matches[0][0])
(out / "step10c_b_scheduler_functions.h").write_text(
    "\n\n".join(parts), encoding="utf-8"
)
subprocess.run([
    "cl", "/nologo", "/std:c11", "/W4", "/WX", "/wd4244", "/wd4702",
    "/D_CRT_SECURE_NO_WARNINGS", "/DWIN32", "/DWIN32CON",
    "/I" + str(repo / "include"), "/I" + str(repo / "submodules/lua"),
    "/I" + str(out), "/Fe:" + str(out / "step10c_b_scheduler.exe"),
    str(repo / "test/test_step10c_b_scheduler.c"),
], cwd=out, check=True)
run = subprocess.run([str(out / "step10c_b_scheduler.exe")], cwd=out,
                     check=True, capture_output=True, text=True)
rows = []
for line in run.stdout.splitlines():
    if not line.startswith("SEED|"):
        continue
    fields = line.split("|")
    row = {"seed": int(fields[1])}
    row.update((k, int(v)) for k, v in (item.split("=", 1)
                                        for item in fields[2:]))
    rows.append(row)
assert [row["seed"] for row in rows] == list(range(1, 1001))
distribution = {
    "parent_P": dict(sorted(Counter(row["P"] for row in rows).items())),
    "leth_a_variant_2": sum(row["A"] for row in rows),
    "leth_c_variant_2": sum(row["C"] for row in rows),
    "leth_d_variant_2": sum(row["D"] for row in rows),
    "nkai_a_variant_2": sum(row["N"] for row in rows),
    "dispensary_parent_N": dict(sorted(Counter(row["DISP"] for row in rows).items())),
}
(out / "frequency-distribution.json").write_text(
    json.dumps(distribution, indent=2), encoding="utf-8"
)
assert "PASS Step 10C-B exactly 1000 deterministic seeds with idempotence" in run.stdout
assert "PASS Step 10C-B representative P=30/middle/P=199 topology and depths" in run.stdout
print("PASS Step 10C-B exactly 1,000 seeds; deterministic idempotence; "
      f"{len(distribution['parent_P'])} observed parent depths; "
      f"distribution={out / 'frequency-distribution.json'}")
