"""Compile the Step 10B3-3 artifact cores outside the repository."""
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


def extract(path, signature):
    source = (repo / path).read_text(encoding="utf8")
    match = re.search(r"(?m)^" + signature + r"[\s\S]*?^}", source)
    assert match, f"missing production helper {signature!r} in {path}"
    return match[0]


helpers = [
    extract("src/read.c", r"int\nnecronomicon_operation_pw_cost"),
    extract("src/read.c", r"int\nnecronomicon_operation_monster"),
    extract("src/artifact.c", r"boolean\nsilver_key_destination_valid"),
    extract("src/artifact.c", r"boolean\nsilver_key_choose_destination"),
]
(out / "step10b3_3_helpers.h").write_text(
    "\n\n".join(helpers) + "\n", encoding="utf8")

test = (repo / "test/test_step10b3_3.c").read_text(encoding="utf8")
test = test.replace("int\nmain(void)",
                    '#include "step10b3_3_helpers.h"\n\nint\nmain(void)')
(out / "test_step10b3_3.c").write_text(test, encoding="utf8")

compile_args = [
    "cl", "/nologo", "/std:c11", "/W4", "/WX",
    "/D_CRT_SECURE_NO_WARNINGS", "/DWIN32", "/DWIN32CON",
    "/I" + str(repo / "include"), "/I" + str(repo / "submodules/lua"),
    "/I" + str(out), "/Fe:" + str(out / "step10b3_3.exe"),
    str(out / "test_step10b3_3.c"), str(repo / "src/objects.c")
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
    subprocess.run(command, cwd=out, check=True, shell=True)

result = subprocess.check_output([str(out / "step10b3_3.exe")], cwd=out,
                                 text=True)
assert "PASS Step 10B3-3" in result
print(result, end="")
