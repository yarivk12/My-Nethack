"""Compile IDs at the frozen Step 9 baseline and test Step 10 save scaffolding.

Run in a VS developer shell: py -3 test/run_step10b_compatibility.py _qa\OUTPUT.
Generated fixtures and binaries stay under the project _qa directory. No game topology.
"""
from pathlib import Path
import io
import re
import subprocess
import sys
import tarfile

BASE = "48fe150af4a087fd2f4ff576b43c1c96cc08c6fe"
repo = Path(__file__).resolve().parents[1]
artifact_root = repo / "_qa"
out = Path(sys.argv[1]).resolve()
if out == repo or (out != artifact_root and artifact_root not in out.parents):
    raise SystemExit("Use an output directory under the project _qa directory")
out.mkdir(parents=True, exist_ok=True)


def function(path, name):
    source = (repo / path).read_text(encoding="utf8")
    matches = re.findall(r"(?m)^(?:staticfn )?\w+\n" + name
                         + r"\([\s\S]*?^\}", source)
    assert len(matches) == 1, (path, name)
    return matches[0]


# Frozen headers establish EVERY old ID, not just a handful of sentinels.
archive = subprocess.check_output(["git", "archive", BASE, "include"], cwd=repo)
baseline = out / "baseline"
baseline.mkdir(exist_ok=True)
with tarfile.open(fileobj=io.BytesIO(archive)) as tar:
    # Only trusted pinned repository headers, never arbitrary archive paths.
    for member in tar.getmembers():
        target = (baseline / member.name).resolve()
        assert baseline in target.parents
        assert member.isdir() or member.isfile()
    tar.extractall(baseline)

source = (repo / "src/sfstruct.c").read_text(encoding="utf8")
macros = source[source.index("#define SFO_BODY"):source.index('#include "sfmacros.h"')]
parts = [function("src/sfstruct.c", "sfstruct_read_error"), macros]
for name in ("branch", "dungeon", "s_level", "levelflags", "you", "obj"):
    parts.append(function("src/sfbase.c", "norm_ptrs_" + name))
    parts.append("SF_C(struct, " + name + ")")
parts.append(function("src/version.c", "check_version"))
(out / "step10b_save_functions.h").write_text("\n".join(parts), encoding="utf8")


def compile_run(label, includes, defines):
    exe = out / (label + ".exe")
    subprocess.run(["cl", "/nologo", "/std:c11", "/W4", "/WX",
                    "/D_CRT_SECURE_NO_WARNINGS", "/DWIN32", "/DWIN32CON",
                    *defines, *["/I" + str(p) for p in includes],
                    "/I" + str(repo / "include"),
                    "/I" + str(repo / "submodules/lua"), "/I" + str(out),
                    "/Fe:" + str(exe), "/Fo:" + str(out / (label + ".obj")),
                    str(repo / "test/test_step10b_compatibility.c")],
                   cwd=out, check=True)
    result = subprocess.check_output([str(exe)], cwd=out, text=True)
    assert exe.is_file(), (label, "test executable disappeared; inspect endpoint protection")
    assert result.startswith("ID monster "), (label, "missing test output; no runtime PASS evidence")
    if label != "step10b_baseline":
        assert "PASS native historical you/levelflags/object save codecs" in result
        assert "PASS production check_version" in result
    ids = {}
    for line in result.splitlines():
        if line.startswith("ID "):
            _, kind, name, value = line.split()
            assert (kind, name) not in ids
            ids[kind, name] = int(value)
        else:
            print(line, flush=True)
    return ids


old = compile_run("step10b_baseline", [baseline / "include"],
                  ["/DSTEP10B_BASELINE"])
new = compile_run("step10b_compatibility", [], [])
for key, value in old.items():
    assert new.get(key) == value, (key, value, new.get(key))
print("PASS append-only IDs:", len(old), "baseline declarations unchanged")

# The reset is an essential part of per-level flag isolation.
assert "svl.level.flags.lethe = 0;" in function("src/mklev.c", "clear_level_structures")
assert "Sfo_levelflags(nhfp, &svl.level.flags" in (repo / "src/save.c").read_text()
assert "Sfi_levelflags(nhfp, &svl.level.flags" in (repo / "src/restore.c").read_text()
dungeon_source = (repo / "src/dungeon.c").read_text()
save_source = (repo / "src/save.c").read_text()
restore_source = (repo / "src/restore.c").read_text()
assert "Sfo_dungeon(nhfp, &svd.dungeons[i]" in dungeon_source
assert "Sfi_dungeon(nhfp, &svd.dungeons[i]" in dungeon_source
assert "Sfo_branch(nhfp, curr, \"branch\")" in dungeon_source
assert "Sfi_branch(nhfp, curr, \"branch\")" in dungeon_source
assert "Sfo_s_level(nhfp, tmplev, \"levchn-s_level\")" in save_source
assert "Sfi_s_level(nhfp, tmplev, \"levchn-s_level\")" in restore_source
assert "SF_C(struct, you)" in (repo / "include/sfmacros.h").read_text()
print("PASS production level/topology native save/restore routing")
