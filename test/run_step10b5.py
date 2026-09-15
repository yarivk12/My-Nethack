"""Run the unified x64 whole-Step-10B integration and closeout gate under _qa."""
from pathlib import Path
import shutil
import subprocess
import sys


repo = Path(__file__).resolve().parents[1]
artifact_root = repo / "_qa"
out = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else Path(
    artifact_root / "step10b-tests" / "x64" / "step10b5-final")
release = (Path(sys.argv[2]).resolve() if len(sys.argv) > 2 else
           repo / "binary/Release/x64")
donor = (Path(sys.argv[3]).resolve() if len(sys.argv) > 3 else
         artifact_root / "dnethack-donor-pinned")
if out == repo or (out != artifact_root and artifact_root not in out.parents):
    raise SystemExit("Use an output directory under the project _qa directory")
if not (release / "NetHack.exe").is_file():
    raise SystemExit("Missing packaged x64 NetHack.exe")
if not donor.is_dir():
    raise SystemExit("Missing pinned dNetHack donor checkout")
out.mkdir(parents=True, exist_ok=True)

vsdev = Path(
    r"C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat")
failures = []


def run_command(label, command, *, needs_msvc=False):
    print("=== " + label + " ===", flush=True)
    try:
        if needs_msvc and not shutil.which("cl"):
            if not vsdev.is_file():
                raise RuntimeError("Missing Visual Studio developer environment")
            payload = subprocess.list2cmdline(command)
            subprocess.run('call "' + str(vsdev)
                           + '" -arch=x64 -host_arch=amd64 && ' + payload,
                           cwd=repo, check=True, shell=True)
        else:
            subprocess.run(command, cwd=repo, check=True)
    except (OSError, RuntimeError, subprocess.CalledProcessError) as error:
        failures.append((label, str(error)))
        print("FAIL " + label + ": " + str(error), flush=True)
        return False
    print("PASS " + label, flush=True)
    return True


def run_python(label, script, *args, needs_msvc=False):
    return run_command(label,
                       [sys.executable, "-B", str(repo / script),
                        *(str(arg) for arg in args)],
                       needs_msvc=needs_msvc)


cumulative_ok = run_python(
    "B1-B4 cumulative integration",
    "test/run_step10b4_final.py", out / "b1_b4", release,
    needs_msvc=True)
cross_ok = run_python(
    "whole-Step-10B cross-phase source contract",
    "test/test_step10b5_source.py")
depth_ok = run_python(
    "depth and save-layout regression",
    "test/run_depth_range.py", out / "depth", needs_msvc=True)

ledger_command = [
    "powershell", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File",
    str(repo / "test/run_ledger_runtime.ps1"),
    "-OutputDirectory", str(out / "ledger"),
    "-RecoverExecutable", str(release / "recover.exe"),
]
ledger_ok = run_command("recovery-file regression", ledger_command,
                        needs_msvc=True)

regression_ok = True
for label, script, args in (
    ("Step 9C foundation", "test/run_step9c_foundation.py",
     (out / "step9c_foundation",)),
    ("Step 9C armor-size", "test/run_step9c_armor_size.py",
     (donor, out / "step9c_armor_size")),
    ("Step 9C coatings", "test/run_step9c_coatings.py",
     (out / "step9c_coatings",)),
    ("Step 9C defense", "test/run_step9c_defense.py",
     (out / "step9c_defense",)),
    ("Step 9C weapon damage", "test/run_step9c_weapon_damage.py",
     (donor, out / "step9c_weapon_damage")),
    ("Step 9C weapon selection", "test/run_step9c_weapon_selection.py",
     (out / "step9c_weapon_selection",)),
    ("Step 9C handedness", "test/run_step9c_handedness.py",
     (donor, out / "step9c_handedness")),
    ("Step 9C spells", "test/run_step9c_spells.py",
     (donor, out / "step9c_spells")),
):
    regression_ok = (run_python(label, script, *args, needs_msvc=True)
                     and regression_ok)

categories = (
    ("identity/table invariants", cumulative_ok and cross_ok),
    ("B1", cumulative_ok and depth_ok and ledger_ok),
    ("B2", cumulative_ok and regression_ok),
    ("B3", cumulative_ok and regression_ok),
    ("B4", cumulative_ok and cross_ok),
    ("object/artifact/Lethe cross-integration", cumulative_ok and cross_ok),
    ("monster/equipment/spell cross-integration",
     cumulative_ok and cross_ok and regression_ok),
    ("lifecycle/save/restore/recovery/bones",
     cumulative_ok and cross_ok and depth_ok and ledger_ok),
    ("ordinary/non-Step-10 isolation",
     cumulative_ok and cross_ok and regression_ok),
    ("Step-10C dormancy/boundary", cross_ok),
    ("generated/glyph/tile consistency", cumulative_ok),
)
print("=== Step 10B5 summary ===", flush=True)
for label, ok in categories:
    print(("PASS " if ok else "FAIL ") + label, flush=True)

if failures:
    print("FAILED SUBGATES:", flush=True)
    for label, error in failures:
        print("- " + label + ": " + error, flush=True)
    raise SystemExit(1)
print("PASS Step 10B5 whole-Step-10B x64 master integration gate")
