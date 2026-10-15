"""Build the real engine and run Step 22 plus the full Step 15-21 fixture set."""
from pathlib import Path
import argparse, os, subprocess, shutil, ctypes
ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--label', default='native')
parser.add_argument('--suite', choices=('sources','combat','utility','items','defense','defense-order','lifecycle','terrain','registry','status-combat','corpse-stress','all'))
args = parser.parse_args()
if os.name == 'nt':
    # Child diagnostic faults belong in evidence logs, not desktop dialogs.
    ctypes.windll.kernel32.SetErrorMode(0x0001 | 0x0002 | 0x8000)
if not args.label.replace('-','').replace('_','').isalnum():
    parser.error('invalid label')
out = ROOT/'_qa/step22'
out.mkdir(parents=True, exist_ok=True)
env = {k.upper():v for k,v in os.environ.items()}
locator = Path(env['PROGRAMFILES(X86)'])/'Microsoft Visual Studio/Installer/vswhere.exe'
vs = Path(subprocess.check_output([str(locator),'-latest','-products','*','-property','installationPath'],text=True).strip())
command=[str(vs/'MSBuild/Current/Bin/MSBuild.exe'),'sys/windows/vs/NetHack/NetHack.vcxproj','/p:Configuration=Release','/p:Platform=x64','/p:STEP15_TEST=true','/v:minimal','/nologo']
for key,folder in [('BinDir','bin'),('ObjDir','obj'),('SymbolsDir','symbols')]:
    command.append(f'/p:{key}={(out/folder).as_posix()}/')
buildlog=out/f'{args.label}-build.log'
with buildlog.open('w') as log:
    result=subprocess.run(command,cwd=ROOT,env=env,stdout=log,stderr=subprocess.STDOUT)
if result.returncode:
    print(buildlog.read_text()[-10000:]);raise SystemExit(result.returncode)
for filename in ('nhdat500','Guidebook.txt','license','nethack.txt','nethackrc.template','opthelp','symbols.template','sysconf.template'):
    shutil.copy2(ROOT/'binary/Release/x64'/filename,out/'bin'/filename)
(out/'bin/record').touch(exist_ok=True)
(out/'bin/sysconf').write_text('WIZARDS=*\nPORTABLE_DEVICE_PATHS=1\n')
env.update(NETHACK_STEP15_TEST='1',STEP21_TRANSITIONS='1',NETHACKOPTIONS='!news,!legacy,!tutorial,!tips')
if args.suite: env['STEP22_FOCUS'] = args.suite
runlog=out/f'{args.label}-runtime.log'
with runlog.open('w') as log:
    result=subprocess.run([str(out/'bin/NetHack.exe')],cwd=out/'bin',env=env,stdout=log,stderr=subprocess.STDOUT,timeout=180)
print(runlog.read_text()[-10000:])
if not result.returncode and 'PASS Step 15 native runtime fixtures' not in runlog.read_text():
    raise SystemExit('Fixture suite did not complete')
raise SystemExit(result.returncode)
