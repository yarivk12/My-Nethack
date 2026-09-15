"""Compile the Step 10B3-2 artifact/helper gate outside the repository."""
from pathlib import Path
import re
import shutil
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
artifact_root = repo / "_qa"
out = Path(sys.argv[1]).resolve()
assert out != repo and (out == artifact_root or artifact_root in out.parents)
out.mkdir(parents=True, exist_ok=True)

source = (repo / "src/questpgr.c").read_text(encoding="utf8")
helper = re.search(r"(?m)^int\nstep10b_alhoon_key_choice[\s\S]*?^}", source)
assert helper, "missing production Alhoon key selector"
(out / "step10b3_2_helpers.h").write_text(helper[0], encoding="utf8")

test = (repo / "test/test_step10b3_2.c").read_text(encoding="utf8")
test = test.replace("int\nmain(void)",
                    '#include "step10b3_2_helpers.h"\n\nint\nmain(void)')
(out / "test_step10b3_2.c").write_text(test, encoding="utf8")
compile_args = [
    "cl", "/nologo", "/std:c11", "/W4", "/WX",
    "/D_CRT_SECURE_NO_WARNINGS", "/DWIN32", "/DWIN32CON",
    "/I" + str(repo / "include"), "/I" + str(repo / "submodules/lua"),
    "/I" + str(out), "/Fe:" + str(out / "step10b3_2.exe"),
    str(out / "test_step10b3_2.c"), str(repo / "src/objects.c")
]
if shutil.which("cl"):
    subprocess.run(compile_args, cwd=out, check=True)
else:
    vsdev = Path(r"C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat")
    assert vsdev.exists(), "cl.exe or the bundled Visual Studio environment is unavailable"
    arch = "x86" if out.name.lower() == "win32" else "x64"
    command = ('call "' + str(vsdev) + '"'
               + " -arch=" + arch + " -host_arch=amd64 && "
               + subprocess.list2cmdline(compile_args))
    # A direct shell string preserves the batch-file quotes; passing the /c
    # payload as a Python argv element makes cmd.exe see backslash-escaped
    # quotes on Windows.
    subprocess.run(command, cwd=out, check=True, shell=True)
result = subprocess.check_output([str(out / "step10b3_2.exe")], cwd=out,
                                 text=True)
assert "PASS Step 10B3-2" in result
print(result, end="")
