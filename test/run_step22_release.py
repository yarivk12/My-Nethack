# My-NetHack modification notice, added 2026-10-15.
# Local project integration/modifications; original authorship is retained.
# Recorded project change dates (Git author dates): 2026-10-15.
# See doc/license-compliance.md and doc/my-nethack-modifications.json.
# Existing copyright, attribution, license and warranty notices still apply.
"""Build the production Release x64 solution, without controlled test grants."""
from pathlib import Path
import ctypes
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / '_qa/step22'
OUT.mkdir(parents=True, exist_ok=True)
env = {key.upper(): value for key, value in os.environ.items()}
if os.name == 'nt':
    ctypes.windll.kernel32.SetErrorMode(0x0001 | 0x0002 | 0x8000)
locator = Path(env['PROGRAMFILES(X86)']) / 'Microsoft Visual Studio/Installer/vswhere.exe'
vs = Path(subprocess.check_output([str(locator), '-latest', '-products', '*',
                                  '-property', 'installationPath'], text=True).strip())
command = [str(vs / 'MSBuild/Current/Bin/MSBuild.exe'),
           'sys/windows/vs/NetHack.sln', '/p:Configuration=Release',
           '/p:Platform=x64', '/p:STEP15_TEST=false', '/m:1', '/v:minimal', '/nologo']
# Keep sandbox-sensitive library tracking files in fresh per-project folders.
# This affects intermediate output only, and leaves repository build files intact.
paths = OUT / 'release-paths.props'
paths.write_text('''<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
<PropertyGroup>
<ObjDir>''' + (OUT / 'releaseobj').as_posix() + '''/$(MSBuildProjectName)/</ObjDir>
<IntDir>$(ObjDir)</IntDir>
<SymbolsDir>''' + (OUT / 'releasesymbols').as_posix() + '''/$(MSBuildProjectName)/</SymbolsDir>
</PropertyGroup></Project>''', encoding='utf8')
command.append('/p:CustomBeforeMicrosoftCommonTargets=' + paths.as_posix())
attempt = 1
logpath = OUT / 'release-package-build.log'
while logpath.exists():
    attempt += 1
    logpath = OUT / f'release-package-build-{attempt}.log'
with logpath.open('w', encoding='utf8') as log:
    result = subprocess.run(command, cwd=ROOT, env=env, stdout=log,
                            stderr=subprocess.STDOUT)
print(logpath.read_text(encoding='utf8')[-5000:])
if result.returncode:
    raise SystemExit(result.returncode)
for name in ('NetHack.exe', 'NetHackW.exe', 'recover.exe', 'nhdat500'):
    assert (ROOT / 'binary/Release/x64' / name).is_file(), name
print('PASS Step22 production Release x64 console, GUI, utilities and packaged data build')
