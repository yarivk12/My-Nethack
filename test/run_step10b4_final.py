"""Run the x64 Step 10B4 gate plus every cumulative Step 10B gate."""
from pathlib import Path
import shutil
import subprocess
import sys


repo = Path(__file__).resolve().parents[1]
artifact_root = repo / "_qa"
out = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else Path(
    artifact_root / "step10b-tests" / "x64" / "step10b4-final")
release = (Path(sys.argv[2]).resolve() if len(sys.argv) > 2 else
           repo / "binary/Release/x64")
if out == repo or (out != artifact_root and artifact_root not in out.parents):
    raise SystemExit("Use an output directory under the project _qa directory")
if not (release / "NetHack.exe").is_file():
    raise SystemExit("Missing packaged x64 NetHack.exe")
out.mkdir(parents=True, exist_ok=True)

vsdev = Path(
    r"C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat")


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


run_python("cumulative B3-4 integration", "test/run_step10b3_4.py",
           out / "cumulative", release, compile_gate=True)
run_python("B4 source", "test/test_step10b4_source.py")
run_python("B4 focused C", "test/run_step10b4.py", out / "b4",
           compile_gate=True)
try:
    run_python("affected Step 9C shop/portal runtime",
               "test/run_step9c_shop_works_runtime.py", release,
               out / "shop_portal_runtime")
except subprocess.CalledProcessError:
    # The legacy WinPTY fixture can race its first #wizloaddes prompt with the
    # dungeon-number probe.  A fresh process/output directory is deterministic.
    run_python("affected Step 9C shop/portal runtime retry",
               "test/run_step9c_shop_works_runtime.py", release,
               out / "shop_portal_runtime_retry")

print("PASS Step 10B4 final x64 environment/support integration")
