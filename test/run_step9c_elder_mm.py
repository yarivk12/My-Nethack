"""Exercise actual elder monster-target selection guards and spell dispatch."""
from pathlib import Path
import re
import subprocess
import sys
repo=Path(__file__).resolve().parents[1]
out=Path(sys.argv[1]).resolve();out.mkdir(parents=True,exist_ok=True)
source=(repo/'src/mcastu.c').read_text(encoding='utf8')
makemon=(repo/'src/makemon.c').read_text(encoding='utf8')
parts=[re.search(r'(?m)^(?:boolean|void)\n'+n+r'\([\s\S]*?^\}',makemon)[0]
       for n in ['step10b_is_witch','step10b_witch_needs_familiar']]
parts.append(re.search(r'(?m)^static int mon_wizard_spells\[\] = \{[\s\S]*?^\};',source)[0])
parts += [re.search(r'(?m)^(?:staticfn )?\w+\n'+n+r'\([\s\S]*?^\}',source)[0]
          for n in ['mith_mm_useless','step10b_spell_cooldown',
                    'step10b_species_spell','step10b_choose_species_spell',
                    'mith_mm_choose_spell','mith_mm_curse','mith_castmm']]
parts[-1]=parts[-1].replace('mith_mm_choose_spell(caster, target)', 'test_choose_spell(caster, target)')
(out/'step9c_elder_mm.h').write_text('\n'.join(parts),encoding='utf8')
subprocess.run(['cl','/nologo','/std:c11','/W4','/D_CRT_SECURE_NO_WARNINGS','/DWIN32','/DWIN32CON',
                '/I'+str(repo/'include'),'/I'+str(repo/'submodules/lua'),'/I'+str(out),
                '/Fe:'+str(out/'step9c_elder_mm.exe'),str(repo/'test/test_step9c_elder_mm.c'),
                str(repo/'src/monst.c'),str(repo/'src/objects.c')],cwd=out,check=True)
subprocess.run([str(out/'step9c_elder_mm.exe')],cwd=out,check=True)
