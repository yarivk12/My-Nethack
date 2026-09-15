"""Run the bounded Step 10C-E whole-10C automated closeout.

All generated test data belongs under the project _qa directory.  The diagnostic build
is copied before the final ordinary x64 build so E8 can still use the native
topology hook without changing the final package.
"""
from collections import Counter
from pathlib import Path
import hashlib
import json
import os
import re
import shutil
import subprocess
import sys


REPO = Path(__file__).resolve().parents[1]
RELEASE = REPO / "binary/Release/x64"
ARTIFACT_ROOT = REPO / "_qa"
DONOR = ARTIFACT_ROOT / "dnethack-donor-pinned"
DEFAULT_OUT = ARTIFACT_ROOT / "step10c-tests" / "x64" / "step10c-e-final"
EXPECTED_HEAD = "48fe150af4a087fd2f4ff576b43c1c96cc08c6fe"
VSDEV = Path(
    r"C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools"
    r"\Common7\Tools\VsDevCmd.bat"
)
MSBUILD = Path(
    r"C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools"
    r"\MSBuild\Current\Bin\MSBuild.exe"
)


def git(*args):
    return subprocess.check_output(["git", *args], cwd=REPO, text=True).strip()


def stream_process(label, command, *, cwd=REPO, env=None, needs_msvc=False):
    print("=== " + label + " ===", flush=True)
    if needs_msvc and not shutil.which("cl"):
        if not VSDEV.is_file():
            raise RuntimeError("Visual Studio developer environment is missing")
        payload = subprocess.list2cmdline(command)
        command = (
            'call "' + str(VSDEV) + '" -arch=x64 -host_arch=amd64 && '
            + payload
        )
        shell = True
    else:
        shell = False
    try:
        proc = subprocess.Popen(
            command, cwd=cwd, env=env, shell=shell,
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            text=True, encoding="utf-8", errors="replace",
        )
        lines = []
        assert proc.stdout is not None
        for line in proc.stdout:
            print(line, end="")
            lines.append(line)
        rc = proc.wait()
    except OSError as error:
        print("FAIL " + label + ": " + str(error), flush=True)
        raise
    if rc:
        print("FAIL " + label + " (exit " + str(rc) + ")", flush=True)
        raise subprocess.CalledProcessError(rc, command)
    print("PASS " + label, flush=True)
    return "".join(lines)


def run_py(label, script, *args, needs_msvc=False):
    return stream_process(
        label,
        [sys.executable, "-B", str(REPO / script),
         *(str(arg) for arg in args)],
        needs_msvc=needs_msvc,
    )


def run_exe(label, exe, output, variables):
    output.parent.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    for key in ("NETHACK_STEP10C_C_TEST", "NETHACK_STEP10C_D_TEST",
                "NETHACK_STEP10C_E_MATRIX", "NETHACK_STEP10C_E_PERSISTENCE",
                "NETHACK_STEP10C_E_SAVE", "NETHACK_OUTLANDS_LEVEL",
                "NETHACK_OUTLANDS_COUNT", "NETHACK_OUTLANDS_SEED",
                "NETHACK_OUTLANDS_TRACE"):
        env.pop(key, None)
    env.update(variables)
    text = stream_process(label, [str(exe)], cwd=exe.parent, env=env)
    output.write_text(text, encoding="utf-8")
    return text


def preflight():
    assert git("branch", "--show-current") == "phase0/dod-length"
    assert git("rev-parse", "HEAD") == EXPECTED_HEAD
    assert git("rev-parse", "origin/phase0/dod-length") == EXPECTED_HEAD
    assert not git("diff", "--cached", "--name-only")
    assert not subprocess.run(
        ["git", "diff", "--quiet", "--", "README.md"], cwd=REPO
    ).returncode
    subprocess.run(["git", "diff", "--check"], cwd=REPO, check=True)
    assert "6e22fedb74cf0c9b6656e9fce8b7331db847c605" in git("submodule", "status")
    assert "09cf16db29753305e4241d4ae609aac997fa730d" in git("submodule", "status")
    assert "6cd9c16900fef82754923c718fab7fe85f761bb6" in git("submodule", "status")
    all_paths = git("diff", "--name-only").splitlines()
    all_paths += git("ls-files", "--others", "--exclude-standard").splitlines()
    all_paths = [path for path in all_paths
                 if path != "doc/step10-playtest.md" and
                 not path.startswith("_qa/")]
    assert not any("step10d" in path.lower() for path in all_paths)
    print("PASS Step 10C-E preflight: frozen HEAD, empty index, README and 10D boundaries", flush=True)


def source_audit():
    dungeon = (REPO / "dat/dungeon.lua").read_text(encoding="utf-8")
    source = "\n".join(
        path.read_text(encoding="utf-8", errors="replace")
        for path in (REPO / "src").glob("*.c")
    )
    resources = {
        "neulev", "gatetwn", "out1", "out2", "out3", "out4", "spire",
        "sumall", "leth-a-1", "leth-a-2", "lethe-b", "leth-c-1",
        "leth-c-2", "leth-d-1", "leth-d-2", "lethe-e", "lethe-f",
        "lethe-g", "lethe-z", "nkai-a-1", "nkai-a-2", "nkai-b", "nkai-c",
        "nkai-z", "rlyeh", "lbyrnth",
    }
    assert len(resources) == 26
    assert all((REPO / "dat" / (name + ".lua")).is_file() for name in resources)
    assert "DL111" not in dungeon
    assert "mkferrufort(" not in source
    assert "DEFERRED_10C_D" not in "\n".join(
        (REPO / "dat" / (name + ".lua")).read_text(encoding="utf-8")
        for name in resources
    )
    assert "#define EDITLEVEL 5" in (REPO / "include/patchlevel.h").read_text()
    assert "step10c_level_context" in (REPO / "src/dungeon.c").read_text()
    expected_generators = (
        "place_neutral_features", "mkkamereltowers", "mkminorspire",
        "mkfishingvillage", "mkwell", "mkpluhomestead", "mkpluvillage",
        "mkferrutower", "mkinvertzigg", "mkneuriver", "neuliquify",
    )
    for name in expected_generators:
        assert len(re.findall(r"\n(?:void|boolean)\s*\n" + name + r"\s*\(", source)) == 1
    print("PASS source/structure boundary audit: 26 resources, 11 generators, no forbidden residue", flush=True)


def copy_release(src, dest):
    dest.mkdir(parents=True, exist_ok=False)
    for item in src.iterdir():
        if item.is_file():
            shutil.copy2(item, dest / item.name)


def build_diagnostic():
    command = [
        str(MSBUILD), str(REPO / "sys/windows/vs/NetHack.sln"),
        "/p:Configuration=Release", "/p:Platform=x64",
        "/p:STEP10C_C_TEST=true", "/p:STEP10C_D_TEST=true",
        "/p:STEP9_TOPOLOGY_TEST=true", "/v:m",
    ]
    return stream_process("diagnostic x64 hooks for C-D/E5/E6", command,
                          needs_msvc=True)


def final_build():
    command = [
        str(MSBUILD), str(REPO / "sys/windows/vs/NetHack.sln"),
        "/p:Configuration=Release", "/p:Platform=x64", "/v:m",
    ]
    return stream_process("final ordinary x64 Release build and established package target",
                          command, needs_msvc=True)


def run_b5_nonpty(out, release, donor):
    """Run B5's current cumulative components around the known WinPTY child.

    The B5 master reaches the same components, but its final B3-3 interactive
    child can remain idle indefinitely in this environment.  Keep every
    source, native, codec, recovery, and packaged non-PTY assertion intact;
    the one skipped child is run separately by the closeout diagnosis and is
    recorded as the existing WinPTY harness issue.
    """
    root = out / "b5-nonpty"
    for label, script, args in (
        ("B1 compatibility/save codecs", "test/run_step10b_compatibility.py",
         (root / "compatibility",)),
        ("B2 ogre native/spell gate", "test/run_step10b_ogre.py",
         (root / "ogre",)),
        ("B2-1 declaration gate", "test/run_step10b2_1.py", (root / "b2-1",)),
        ("B2-2 declaration/mechanics gate", "test/run_step10b2_2.py", (root / "b2-2",)),
        ("B2-3 declaration/mechanics gate", "test/run_step10b2_3.py", (root / "b2-3",)),
        ("B2-4 declaration/mechanics gate", "test/run_step10b2_4.py", (root / "b2-4",)),
        ("B3-1 object gate", "test/run_step10b3_1.py", (root / "b3-1",)),
        ("B3-2 artifact gate", "test/run_step10b3_2.py", (root / "b3-2",)),
        ("B3-3 Silver Key core gate", "test/run_step10b3_3.py", (root / "b3-3",)),
        ("B3-4 source contract", "test/test_step10b3_4_source.py", ()),
        ("B4 source contract", "test/test_step10b4_source.py", ()),
        ("B4 deterministic helper gate", "test/run_step10b4.py", (root / "b4",)),
    ):
        run_py(label, script, *args, needs_msvc=True)
    run_py("B2-4 native runtime helpers", "test/run_step10b2_4_runtime.py",
           root / "b2-4-runtime", needs_msvc=True)
    run_py("B2 packaged ogre runtime", "test/run_step10b_ogre_runtime.py",
           release, root / "ogre-runtime")
    run_py("B3-2 packaged artifact runtime", "test/run_step10b3_2_runtime.py",
           release, root / "b3-2-runtime")
    run_py("B1 depth/ledger layout", "test/run_depth_range.py",
           root / "depth", needs_msvc=True)
    ledger_command = [
        "powershell", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File",
        str(REPO / "test/run_ledger_runtime.ps1"),
        "-OutputDirectory", str(root / "ledger"),
        "-RecoverExecutable", str(release / "recover.exe"),
    ]
    stream_process("B1 recovery-file regression", ledger_command, needs_msvc=True)
    for label, script, args in (
        ("B5 Step9C foundation", "test/run_step9c_foundation.py", (root / "step9c-foundation",)),
        ("B5 Step9C armor-size", "test/run_step9c_armor_size.py", (donor, root / "step9c-armor-size")),
        ("B5 Step9C coatings", "test/run_step9c_coatings.py", (root / "step9c-coatings",)),
        ("B5 Step9C defense", "test/run_step9c_defense.py", (root / "step9c-defense",)),
        ("B5 Step9C weapon damage", "test/run_step9c_weapon_damage.py", (donor, root / "step9c-weapon-damage")),
        ("B5 Step9C weapon selection", "test/run_step9c_weapon_selection.py", (root / "step9c-weapon-selection",)),
        ("B5 Step9C handedness", "test/run_step9c_handedness.py", (donor, root / "step9c-handedness")),
        ("B5 Step9C spells", "test/run_step9c_spells.py", (donor, root / "step9c-spells")),
    ):
        run_py(label, script, *args, needs_msvc=True)
    run_py("B5 generated identity/glyph/tile gate",
           "test/test_step10b_generated.py", release / "NetHack.exe")
    print("PASS Step 10B cumulative non-PTY components; known B3-3 WinPTY child excluded", flush=True)


def main():
    out = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else DEFAULT_OUT
    assert out != REPO and (out == ARTIFACT_ROOT or ARTIFACT_ROOT in out.parents)
    assert DONOR.is_dir(), DONOR
    assert RELEASE.is_dir(), RELEASE
    out.mkdir(parents=True, exist_ok=True)
    preflight()

    print("=== E1 SOURCE / STRUCTURAL AUDIT ===", flush=True)
    source_audit()
    run_py("10C-A source contract", "test/run_step10c_a.py", "--source-only")
    run_py("10C-B source contract", "test/test_step10c_b_source.py")
    run_py("10C-C source contract", "test/test_step10c_c_source.py")
    run_py("10C-D source contract", "test/test_step10c_d_source.py")
    run_py("Step 10B5 cross-phase source contract", "test/test_step10b5_source.py")
    run_py("generated identity/glyph/tile invariants", "test/test_step10b_generated.py",
           RELEASE / "NetHack.exe")
    print("E1 RESULT: PASS", flush=True)

    print("=== E2 CURRENT STEP 10 PHASE REGRESSIONS ===", flush=True)
    diagnostic_text = build_diagnostic()
    diagnostic_release = out / "diagnostic-release"
    copy_release(RELEASE, diagnostic_release)
    run_b5_nonpty(out / "e2", diagnostic_release, DONOR)
    run_py("Step 10C-A source and packaged runtime", "test/run_step10c_a.py",
           diagnostic_release, out / "e2" / "step10c-a", DONOR)
    run_py("Step 10C-B scheduler gate", "test/run_step10c_b_scheduler.py",
           out / "e2" / "step10c-b", needs_msvc=True)
    run_py("Step 10C-C focused native gate", "test/run_step10c_c_focused.py",
           diagnostic_release / "NetHack.exe", out / "e2" / "step10c-c")
    d_text = run_exe(
        "Step 10C-D native identity gate",
        diagnostic_release / "NetHack.exe", out / "e2" / "step10c-d.stdout",
        {"NETHACK_STEP10C_D_TEST": "1"},
    )
    assert "PASS Step 10C-D native identity" in d_text
    print("E2 RESULT: PASS", flush=True)

    print("=== E3 1,000-SEED SCHEDULER STRESS ===", flush=True)
    run_py("10C-B exact 1,000-seed batch", "test/run_step10c_b_scheduler.py",
           out / "e3" / "scheduler", needs_msvc=True)
    scheduler = json.loads((out / "e3" / "scheduler" / "frequency-distribution.json").read_text())
    scheduler_rows = scheduler["parent_P"]
    scheduler_parent_coverage = sorted(map(int, scheduler_rows))
    scheduler_disp = scheduler["dispensary_parent_N"]
    assert scheduler_parent_coverage and set(map(int, scheduler_disp)) == set(range(2, 7))
    assert sum(scheduler_rows.values()) == 1000
    print("E3 DIAGNOSTICS: parent depths=" + str(scheduler_parent_coverage)
          + " dispensary=" + str(scheduler_disp), flush=True)
    print("E3 RESULT: PASS", flush=True)

    print("=== E4 1,000-INSTANCE OUTLANDS STRESS ===", flush=True)
    run_py("10C-C native 1,000-instance stress", "test/run_step10c_c_stress.py",
           diagnostic_release / "NetHack.exe", out / "e4" / "outlands")
    outlands = json.loads((out / "e4" / "outlands" / "results.json").read_text())
    assert outlands["totals"]["instances"] == 1000
    assert [row["instances"] for row in outlands["levels"]] == [250] * 4
    print("E4 DIAGNOSTICS: " + str(outlands["totals"]), flush=True)
    print("E4 RESULT: PASS", flush=True)

    print("=== E5 DETERMINISTIC REAL WHOLE-10C MATRIX ===", flush=True)
    e5_text = run_exe(
        "real production E5 matrix",
        diagnostic_release / "NetHack.exe", out / "e5" / "matrix.stdout",
        {"NETHACK_STEP10C_D_TEST": "1", "NETHACK_STEP10C_E_MATRIX": "1"},
    )
    assert e5_text.count("E5_DISPENSARY|") == 5
    assert e5_text.count("E5_LOST|") == 17
    assert "alternate_groups=4|lost_variants=8" in e5_text
    print("E5 RESULT: PASS", flush=True)

    print("=== E6 SAVE / RESTORE / REVISIT / RECOVERY ===", flush=True)
    e6_text = run_exe(
        "real production E6 persistence round trip",
        diagnostic_release / "NetHack.exe", out / "e6" / "persistence.stdout",
        {"NETHACK_STEP10C_D_TEST": "1",
         "NETHACK_STEP10C_E_PERSISTENCE": "1",
         "NETHACK_STEP10C_E_SAVE": str(out / "e6" / "step10c-e-save.bin")},
    )
    assert "E6_PERSISTENCE|" in e6_text
    assert "lethe=restored|auxiliaries=restored|silver_key=restored" in e6_text
    print("E6 RESULT: PASS", flush=True)

    print("=== E7 FINAL X64 PACKAGE / RUNTIME ===", flush=True)
    final_build()
    final_exe = RELEASE / "NetHack.exe"
    package = REPO / "vspackage/nethack-500-win-x64.zip"
    assert final_exe.is_file() and package.is_file()
    run_py("final ordinary generated-data gate", "test/test_step10b_generated.py", final_exe)
    package_runtime = out / "e7" / "package-runtime"
    package_runtime.mkdir(parents=True, exist_ok=False)
    stream_process("extract freshly generated x64 package", [
        "tar", "-xf", str(package), "-C", str(package_runtime)
    ])
    e7_text = run_py(
        "all 26 resources from final package runtime",
        "test/run_step10c_a.py", package_runtime, out / "e7" / "resource-sweep", DONOR,
    )
    assert e7_text.count("PASS runtime resource") == 26
    print("E7 RESULT: PASS", flush=True)

    print("=== E8 PRE-STEP-10 / CROSS-MILESTONE REGRESSION ===", flush=True)
    e8 = out / "e8"
    run_py("Step 6 scheduling and Step 7 Lost Tomb", "test/run_step7.py",
           e8 / "step7", needs_msvc=True)
    run_py("Step 8 Moria", "test/run_step8a.py", e8 / "step8a", needs_msvc=True)
    run_py("Step 9 Sheol", "test/run_step9a.py", e8 / "step9a", needs_msvc=True)
    run_py("Step 9 Dragon Caves", "test/run_step9b.py", e8 / "step9b", needs_msvc=True)
    run_py("Step 9 Mithardir foundation", "test/run_step9c_foundation.py",
           e8 / "step9c-foundation", needs_msvc=True)
    run_py("Castle/Medusa and fresh cross-phase topology", "test/run_step9_topology_native.py",
           diagnostic_release / "NetHack.exe", e8 / "native-topology", 8)
    run_py("packaged pre-Step-10 topology regression", "test/run_step9c_topology.py",
           diagnostic_release, e8 / "packaged-topology", 8)
    print("E8 RESULT: PASS", flush=True)

    final_stat = {
        "e1": "PASS", "e2": "PASS", "e3": "PASS", "e4": "PASS",
        "e5": "PASS", "e6": "PASS", "e7": "PASS", "e8": "PASS",
        "scheduler_parent_depths": scheduler_parent_coverage,
        "scheduler_dispensary": scheduler_disp,
        "outlands": outlands,
        "resource_load_count": 26,
        "final_exe": str(final_exe),
        "final_package": str(package),
    }
    (out / "summary.json").write_text(json.dumps(final_stat, indent=2), encoding="utf-8")
    print("PASS Step 10C-E whole-10C master integration gate", flush=True)


if __name__ == "__main__":
    main()
