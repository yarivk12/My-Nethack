"""Compile the Step 10B4 deterministic helper gate under the project _qa directory."""
from pathlib import Path
import re
import subprocess
import sys


repo = Path(__file__).resolve().parents[1]
artifact_root = repo / "_qa"
out = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else Path(
    artifact_root / "step10b-tests" / "x64" / "step10b4")
assert out != repo and (out == artifact_root or artifact_root in out.parents)
out.mkdir(parents=True, exist_ok=True)

parts = []
for path, names in {
    "src/questpgr.c": ("step10b_is_outlands_context",),
    "src/spell.c": ("step10b_hero_spell_chance",),
    "src/mcastu.c": ("step10b_mon_spell_fumble_threshold",
                     "step10b_mon_spell_always_fumbles"),
    "src/dokick.c": ("step10b_tree_kick_has_loot",),
    "src/dig.c": ("step10b_tree_cut_sticks",),
    "src/trap.c": ("step10b_mirror_pit_damage",
                   "step10b_trap_projectile_material",
                   "step10b_lethe_otyp", "step10b_lethe_marker_spe",
                   "step10b_lethe_drain_spe"),
    "src/display.c": ("step10b_terrain_color",),
}.items():
    source = (repo / path).read_text(encoding="utf8")
    for name in names:
        match = re.search(r"(?m)^(?:boolean|int)\n" + name
                          + r"\([\s\S]*?^\}", source)
        if not match:
            raise AssertionError(f"missing helper {name} in {path}")
        parts.append(match[0])

(out / "step10b4_helpers.h").write_text("\n".join(parts), encoding="utf8")
test = (repo / "test/test_step10b4.c").read_text(encoding="utf8")
test = test.replace("#include <stdio.h>",
                    "#include <stdio.h>\n#include \"step10b4_helpers.h\"")
(out / "test_step10b4.c").write_text(test, encoding="utf8")
subprocess.run([
    "cl", "/nologo", "/std:c11", "/W4", "/WX",
    "/D_CRT_SECURE_NO_WARNINGS", "/DWIN32", "/DWIN32CON",
    "/I" + str(repo / "include"), "/I" + str(repo / "submodules/lua"),
    "/I" + str(out), "/Fe:" + str(out / "test_step10b4.exe"),
    str(out / "test_step10b4.c")
], cwd=out, check=True)
result = subprocess.check_output([str(out / "test_step10b4.exe")], cwd=out,
                                 text=True)
assert "PASS Step 10B4" in result
print(result, end="")

state_parts = []
for path, name in (
    ("src/dog.c", "step10b_pet_separation_catchup"),
    ("src/shknam.c", "step10b_designate_plumach_shopkeeper"),
    ("src/priest.c", "step10b_designate_bridge_priest"),
):
    source = (repo / path).read_text(encoding="utf8")
    match = re.search(r"(?m)^(?:boolean|void)\n" + name
                      + r"\([\s\S]*?^\}", source)
    if not match:
        raise AssertionError(f"missing state helper {name} in {path}")
    state_parts.append(match[0])
(out / "step10b4_state_helpers.h").write_text("\n".join(state_parts),
                                               encoding="utf8")
subprocess.run([
    "cl", "/nologo", "/std:c11", "/W4", "/WX", "/wd4244",
    "/D_CRT_SECURE_NO_WARNINGS", "/DWIN32", "/DWIN32CON",
    "/I" + str(repo / "include"), "/I" + str(repo / "submodules/lua"),
    "/I" + str(out), "/Fe:" + str(out / "test_step10b4_state.exe"),
    str(repo / "test/test_step10b4_state.c")
], cwd=out, check=True)
state_result = subprocess.check_output([str(out / "test_step10b4_state.exe")],
                                       cwd=out, text=True)
assert "PASS Step 10B4" in state_result
print(state_result, end="")
