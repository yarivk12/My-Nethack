# My-NetHack modification notice, added 2026-10-15.
# Local project integration/modifications; original authorship is retained.
# Recorded project change dates (Git author dates): 2026-10-10.
# See doc/license-compliance.md and doc/my-nethack-modifications.json.
# Existing copyright, attribution, license and warranty notices still apply.
"""Build and run the native Step 21 suite, including six real level transitions."""
from pathlib import Path
import argparse
import os
import subprocess
import shutil

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--label', default='native')
args = parser.parse_args()
if not args.label.replace('-', '').replace('_', '').isalnum():
    parser.error('label must contain letters, digits, hyphens or underscores')
out = ROOT / '_qa/step21'
out.mkdir(parents=True, exist_ok=True)
env = {key.upper(): value for key, value in os.environ.items()}
locator = Path(env['PROGRAMFILES(X86)']) / 'Microsoft Visual Studio/Installer/vswhere.exe'
vs = Path(subprocess.check_output([str(locator), '-latest', '-products', '*',
                                  '-property', 'installationPath'], text=True).strip())
command = [str(vs / 'MSBuild/Current/Bin/MSBuild.exe'),
           'sys/windows/vs/NetHack/NetHack.vcxproj', '/p:Configuration=Release',
           '/p:Platform=x64', '/p:STEP15_TEST=true', '/v:minimal', '/nologo']
for key, folder in [('BinDir', 'bin'), ('ObjDir', 'obj'), ('SymbolsDir', 'symbols')]:
    command.append(f'/p:{key}={(out / "red" / folder).as_posix()}/')
buildlog = out / f'{args.label}-build.log'
with buildlog.open('w') as log:
    result = subprocess.run(command, cwd=ROOT, env=env, stdout=log, stderr=subprocess.STDOUT)
if result.returncode:
    print(buildlog.read_text()[-5000:])
    raise SystemExit(result.returncode)

# MSBuild emits the executable; the runtime also needs the packaged game data.
for filename in ('nhdat500', 'Guidebook.txt', 'license', 'nethack.txt',
                 'nethackrc.template', 'opthelp', 'symbols.template',
                 'sysconf.template'):
    source = ROOT / 'binary/Release/x64' / filename
    if not source.is_file():
        raise SystemExit(f'Missing runtime resource: {source}')
    shutil.copy2(source, out / 'red/bin' / filename)
(out / 'red/bin/record').touch(exist_ok=True)
env.update(NETHACK_STEP15_TEST='1', STEP21_TRANSITIONS='1',
           NETHACKOPTIONS='!news,!legacy,!tutorial,!tips')
runlog = out / f'{args.label}-runtime.log'
with runlog.open('w') as log:
    result = subprocess.run([str(out / 'red/bin/NetHack.exe')],
                            cwd=out / 'red/bin', env=env, stdout=log,
                            stderr=subprocess.STDOUT, timeout=120)
print(runlog.read_text()[-4500:])
if not result.returncode and 'PASS Step 15 native runtime fixtures' not in runlog.read_text():
    raise SystemExit('Native process exited without completing the fixture suite')
raise SystemExit(result.returncode)
