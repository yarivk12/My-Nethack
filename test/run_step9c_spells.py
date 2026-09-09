"""Compare imported spell selection, dice and mass healing with pinned source."""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
donor, out = map(Path, sys.argv[1:3])
out = out.resolve(); out.mkdir(parents=True, exist_ok=True)
pin = '17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'
original = subprocess.check_output(['git', 'show', pin + ':dnethack-3.4.3/src/mcastu.c'],
                                   cwd=donor).decode('utf8')
spell_header = subprocess.check_output(['git', 'show', pin + ':dnethack-3.4.3/include/spell.h'],
                                       cwd=donor).decode('utf8')
assert re.search(r'#define MAX_BONUS_DICE\s+10\b', spell_header)
source = (repo / 'src/mcastu.c').read_text(encoding='utf8')
parts = [re.search(r'(?m)^staticfn (?:int|void)\n' + name + r'\([\s\S]*?^\}', source)[0]
         for name in ['mith_elder_spell', 'mith_spell_damage', 'mith_mass_cure']]
elder = original.split('case PM_ALABASTER_ELF_ELDER:', 1)[1].split('case PM_WITCH_S_FAMILIAR:', 1)[0]
elder = elder.rsplit('break;', 1)[0]
for name, mapped in {'MASS_CURE_FAR': 'MITH_CURE_FAR', 'MASS_CURE_CLOSE': 'MITH_CURE_CLOSE',
                     'SLEEP': 'MITH_SLEEP', 'DISAPPEAR': 'DISAPPEAR',
                     'CONFUSE_YOU': 'CONFUSE_YOU', 'BLIND_YOU': 'BLIND_YOU',
                     'AGGRAVATION': 'AGGRAVATION'}.items():
    elder = re.sub(r'\b' + name + r'\b', 'MCAST_' + mapped, elder)
parts.append('static int donor_elder(void) { (void)rn2(2);' + elder + '\nreturn -1; }')
dice = original.split('int dmd = 6, dmn = min(MAX_BONUS_DICE, ml/3+1);', 1)[1]
dice = dice.split('dmg = d(dmn, dmd);', 1)[0]
dice = dice.replace('is_alabaster_mummy(mtmp->data)', '(syllable >= 0)')
dice = dice.replace('mtmp->mvar_syllable', 'syllable').replace('SYLLABLE_OF_POWER__KRAU', 'MITH_KRAU')
parts.append('static int donor_damage(struct monst *mtmp, struct attack *mattk) {'
             'int ml=mtmp->m_lev, dmd=6, dmn=min(10,ml/3+1);' + dice + 'return d(dmn,dmd);}')
for name, spell in [('close', 'MASS_CURE_CLOSE'), ('far', 'MASS_CURE_FAR')]:
    body = original.split('case ' + spell + ':{', 1)[1].split('}break;', 1)[0]
    body = body.replace('MAX_BONUS_DICE', '10').replace('dmg = 0;', '')
    parts.append('static void donor_' + name + '(struct monst *mtmp) {' + body + '}')
(out / 'step9c_spells.h').write_text('\n'.join(parts), encoding='utf8')
subprocess.run(['cl', '/nologo', '/std:c11', '/W4', '/D_CRT_SECURE_NO_WARNINGS',
                '/DWIN32', '/DWIN32CON', '/I' + str(repo / 'include'),
                '/I' + str(repo / 'submodules/lua'), '/I' + str(out),
                '/Fe:' + str(out / 'step9c_spells.exe'),
                str(repo / 'test/test_step9c_spells.c'), str(repo / 'src/monst.c')], cwd=out, check=True)
subprocess.run([str(out / 'step9c_spells.exe')], cwd=out, check=True)
