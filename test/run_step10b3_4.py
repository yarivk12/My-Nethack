"""Run the single x64 Step 10B3-4 integration gate."""
from pathlib import Path
import shutil
import subprocess
import sys


repo = Path(__file__).resolve().parents[1]
artifact_root = repo / "_qa"
out = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else Path(
    artifact_root / "step10b-tests" / "x64" / "step10b3-4-final")
release = (Path(sys.argv[2]).resolve() if len(sys.argv) > 2 else
           repo / "binary/Release/x64")
if out == repo or (out != artifact_root and artifact_root not in out.parents):
    raise SystemExit("Use an output directory under the project _qa directory")
if not release.is_dir():
    raise SystemExit("Missing x64 Release directory: " + str(release))
if not (release / "NetHack.exe").is_file():
    raise SystemExit("Missing packaged x64 NetHack.exe")
out.mkdir(parents=True, exist_ok=True)

vsdev = Path(r"C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat")


def run_python(label, script, *args, compile_gate=False):
    print("=== " + label + " ===", flush=True)
    command = [sys.executable, "-B", str(repo / script),
               *(str(arg) for arg in args)]
    if compile_gate and not shutil.which("cl"):
        if not vsdev.is_file():
            raise SystemExit("Missing bundled Visual Studio developer environment")
        payload = subprocess.list2cmdline(["py", "-3", "-B", *command[2:]])
        command_line = ('call "' + str(vsdev)
                        + '" -arch=x64 -host_arch=amd64 && ' + payload)
        subprocess.run(command_line, cwd=repo, check=True, shell=True)
    else:
        subprocess.run(command, cwd=repo, check=True)


for script in (
    "test/test_step10b_source.py",
    "test/test_step10b2_1_source.py",
    "test/test_step10b2_2_source.py",
    "test/test_step10b2_3_source.py",
    "test/test_step10b2_4_source.py",
    "test/test_step10b3_1_source.py",
    "test/test_step10b3_2_source.py",
    "test/test_step10b3_3_source.py",
    "test/test_step10b3_4_source.py",
):
    run_python(Path(script).stem, script)

for name, script in (
    ("compatibility", "test/run_step10b_compatibility.py"),
    ("ogre", "test/run_step10b_ogre.py"),
    ("B2-1", "test/run_step10b2_1.py"),
    ("B2-2", "test/run_step10b2_2.py"),
    ("B2-3", "test/run_step10b2_3.py"),
    ("B2-4", "test/run_step10b2_4.py"),
    ("B3-1", "test/run_step10b3_1.py"),
    ("B3-2", "test/run_step10b3_2.py"),
    ("B3-3", "test/run_step10b3_3.py"),
):
    run_python(name, script, out / name.lower().replace("-", "_"),
               compile_gate=True)

run_python("B2-4 runtime helpers", "test/run_step10b2_4_runtime.py",
           out / "b2_4_runtime", compile_gate=True)
run_python("ogre packaged runtime", "test/run_step10b_ogre_runtime.py",
           release, out / "ogre_runtime")
run_python("B3-2 packaged runtime", "test/run_step10b3_2_runtime.py",
           release, out / "b3_2_runtime")
run_python("B3-3 packaged runtime", "test/run_step10b3_3_runtime.py",
           release, out / "b3_3_runtime")
run_python("generated x64 enum/glyph/tile gate",
           "test/test_step10b_generated.py", release / "NetHack.exe")

print("PASS B3-4 x64 master integration: B3 objects/artifacts, dependent monster paths, persistence, and isolation")
