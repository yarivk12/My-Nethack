"""Build/run Step 13 native fixtures using the repository's MSVC diagnostic pattern."""
from pathlib import Path
import argparse, os, shutil, subprocess
R=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,default=R/'_qa/step13-diagnostic');p.add_argument('--no-build',action='store_true');p.add_argument('--combat-only',action='store_true');p.add_argument('--step16c-only',action='store_true');a=p.parse_args()
out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
if not a.no_build:
 locator=Path(os.environ.get('ProgramFiles(x86)','C:/Program Files (x86)'))/'Microsoft Visual Studio/Installer/vswhere.exe'
 vs=Path(subprocess.check_output([str(locator),'-latest','-products','*','-property','installationPath'],text=True).strip())
 cmd=[str(vs/'MSBuild/Current/Bin/MSBuild.exe'),str(R/'sys/windows/vs/NetHack/NetHack.vcxproj'),'/p:Configuration=Release','/p:Platform=x64','/p:STEP13_TEST=true','/v:minimal','/nologo']
 for key,name in [('BinDir','bin'),('ObjDir','obj'),('SymbolsDir','symbols')]:cmd.append(f'/p:{key}={(out/name).as_posix()}/')
 with (out/'build.log').open('w') as log:subprocess.run(cmd,cwd=R,env=dict(os.environ),stdout=log,stderr=subprocess.STDOUT,check=True)
for name in ['nhdat500','symbols.template','sysconf.template','nethackrc.template','Guidebook.txt','opthelp','license']:
 shutil.copy2(R/'binary/Release/x64'/name,out/'bin'/name)
env={k:v for k,v in os.environ.items() if not k.startswith(('NETHACK_STEP','STEP11_','STEP13_','CUSTOMROOM','SHOPTYPE'))}
env.update(NETHACK_STEP13_TEST='1',NETHACKOPTIONS='!news,!legacy,!tutorial,!tips')
(out/'bin/sysconf').write_text('WIZARDS=*\nPORTABLE_DEVICE_PATHS=1\n')
if a.combat_only:env['STEP13_COMBAT_ONLY']='1'
if a.step16c_only:env['STEP16C_ONLY']='1'
r=subprocess.run([str(out/'bin/NetHack.exe')],cwd=out/'bin',env=env,capture_output=True,timeout=600)
text=(r.stdout+r.stderr).decode(errors='replace');(out/'runtime.log').write_text(text,encoding='utf8')
print('\n'.join(line for line in text.splitlines() if line.startswith(('PASS','SIZE','CORPUS'))));
if r.returncode: print(text[-3000:])
assert r.returncode==0,r.returncode
assert ('PASS Step 17 native integration fixtures' if os.getenv('STEP17_ONLY') else 'PASS Step 16C focused native integration fixtures' if a.step16c_only else 'PASS Step 13 focused combat/knowledge fixtures' if a.combat_only else 'PASS Step 13 native runtime fixtures') in text
