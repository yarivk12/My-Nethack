"""Compare actual Mithardir fey equipment against the pinned donor over seeds."""
from pathlib import Path
import re
import subprocess
import sys

PIN = '17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'
repo = Path(__file__).resolve().parents[1]
donor, out = map(lambda p: Path(p).resolve(), sys.argv[1:3])
out.mkdir(parents=True, exist_ok=True)
source = (repo / 'src/makemon.c').read_text(encoding='utf8')
parts = [re.search(r'(?m)^staticfn \w+\n' + name + r'\([\s\S]*?^\}', source)[0]
         for name in ['mith_fey_item', 'mith_alabaster_weapon', 'mith_fey_equipment']]
source = subprocess.check_output(['git', 'show', PIN + ':dnethack-3.4.3/src/makemon.c'], cwd=donor).decode('utf8')

def block(marker):
    start = source.index('{', source.index(marker))
    depth = 1
    end = start + 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[start + 1:end - 1]

cases = []
for typ in ['ALABASTER_ELF', 'ALABASTER_ELF_ELDER', 'COURE_ELADRIN',
            'NOVIERE_ELADRIN', 'BRALANI_ELADRIN']:
    marker = ('else if(mm == PM_' + typ + '){' if typ.startswith('ALABASTER')
              else 'else if(ptr == &mons[PM_' + typ + ']){')
    cases.append('case PM_' + typ + ':\n' + block(marker) + '\nbreak;')
nymph = block('if(ptr == &mons[PM_THRIAE] || ptr == &mons[PM_OCEANID] || ptr == &mons[PM_SELKIE])')
nymph = re.sub(r'if\(ptr == &mons\[PM_THRIAE\]\)\s*\(void\)mongets\([^;]+;', '', nymph)
cases.append('case PM_SELKIE: case PM_OCEANID:\n' + nymph + '\nbreak;')
body = '\n'.join(cases)
for old, new in [('GLOVES', 'LEATHER_GLOVES'), ('LEATHER_HELM', 'ELVEN_LEATHER_HELM'),
                 ('CLOAK', 'LEATHER_CLOAK')]:
    body = re.sub(r'\b' + old + r'\b', new, body)
body = body.replace('->obj_material', '->obranch_material')
body = re.sub(r'->objsize = ([^;]+);', r'->obranch_size = (\1) + 1;', body)
body = body.replace('fix_object(otmp)', '(otmp->owt = weight(otmp))')
body = body.replace('mongets(', 'donor_mongets(').replace('m_initthrow(', 'donor_initthrow(')
parts.append('static boolean donor_equipment(struct monst *mtmp) {\n'
             'struct permonst *ptr = mtmp->data; struct obj *otmp;\n'
             'switch (monsndx(ptr)) {\n' + body + '\ndefault: return FALSE;} return TRUE;\n}')
(out / 'step9c_fey_equipment.h').write_text('\n'.join(parts), encoding='utf8')
subprocess.run(['cl', '/nologo', '/std:c11', '/W4', '/D_CRT_SECURE_NO_WARNINGS',
                '/DWIN32', '/DWIN32CON', '/I' + str(repo / 'include'),
                '/I' + str(repo / 'submodules/lua'), '/I' + str(out),
                '/Fe:' + str(out / 'step9c_fey_equipment.exe'),
                str(repo / 'test/test_step9c_fey_equipment.c'),
                str(repo / 'src/monst.c'), str(repo / 'src/objects.c')], cwd=out, check=True)
subprocess.run([str(out / 'step9c_fey_equipment.exe')], cwd=out, check=True)
