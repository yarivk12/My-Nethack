"""Actual Aspect turn, spell suppression and moving light-source machinery."""
from pathlib import Path
import re,subprocess,sys
repo=Path(__file__).resolve().parents[1]
donor,out=map(Path,sys.argv[1:3]);out=out.resolve();out.mkdir(parents=True,exist_ok=True)
pin='17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'
def original(file):
    return subprocess.check_output(['git','show',pin+':dnethack-3.4.3/src/'+file],cwd=donor).decode()
parts=[]
parts.append(re.search(r'(?m)^#define LSF_SHOW[^\n]*',(repo/'src/light.c').read_text())[0])
for file,names in {'allmain.c':['mith_regen_increment','mith_syllable_turn'],
                   'light.c':['do_light_sources'],'hacklib.c':['isqrt','dist2']}.items():
    text=(repo/'src'/file).read_text()
    for name in names:
        parts.append(re.search(r'(?m)^(?:staticfn )?\w+\n'+name+r'\([\s\S]*?^\}',text)[0])
vision=(repo/'src/vision.c').read_text()
for name in ['circle_data','circle_start']:
    parts.insert(0,re.search(r'const coordxy '+name+r'\[\] = \{[\s\S]*?\};',vision)[0])
spell=(repo/'src/spell.c').read_text()
block=spell.split('    {\n        struct monst *mon;\n\n        for (mon = fmon;',1)[1]
block='    {\n        struct monst *mon;\n\n        for (mon = fmon;'+block.split('    /* Clamp to percentile */',1)[0]
parts.append('static int local_chance(int chance) {\n'+block+'return max(0,min(100,chance));\n}')
block=original('spell.c').split('\tif(flags.silence_level){',1)[1].split('\tif(u.uz.dnum == neutral_dnum',1)[0]
block='if(silence){'+block.replace('u.unaen_duration','u.mith_timers[MITH_NAEN]')
parts.append('static int donor_chance(int chance, int silence) {\n'+block+'return max(0,min(100,chance));\n}')
assert re.search(r'if\(mtmp->data == &mons\[PM_ASPECT_OF_THE_SILENCE\]\)\{\s*flags.silence_level=1;\s*losepw\(3\);',original('allmain.c'))
(out/'step9c_aspect.h').write_text('\n'.join(parts))
subprocess.run(['cl','/nologo','/std:c11','/W4','/D_CRT_SECURE_NO_WARNINGS','/DWIN32','/DWIN32CON',
 '/I'+str(repo/'include'),'/I'+str(repo/'submodules/lua'),'/I'+str(out),
 '/Fe:'+str(out/'step9c_aspect.exe'),str(repo/'test/test_step9c_aspect.c'),str(repo/'src/monst.c')],cwd=out,check=True)
subprocess.run([str(out/'step9c_aspect.exe')],cwd=out,check=True)
