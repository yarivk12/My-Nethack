"""Compile real damage-path blocks against their pinned Vaul counterparts."""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
donor, out = map(Path, sys.argv[1:3]); out.mkdir(parents=True, exist_ok=True)
pin = '17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'

def upstream(file):
    return subprocess.check_output(['git', 'show', pin + ':dnethack-3.4.3/src/' + file],
                                   cwd=donor).decode('utf8')

def between(text, start, end):
    return text.split(start, 1)[1].split(end, 1)[0]

zap = (repo/'src/zap.c').read_text(encoding='utf8')
blast = (repo/'src/explode.c').read_text(encoding='utf8')
trap = (repo/'src/trap.c').read_text(encoding='utf8')
parts = []
# Extract the actual native paths, retaining their own protection semantics.
# Poison uses native towel protection, not the donor's global physical rule.
paths = [
    ('psychic', 'monmove.c', 'dmg = rnd(15);', 'losehp(dmg, "psychic blast"', 'dmg', 'spell'),
    ('striking', 'muse.c', 'tmp = d(2, 12);', 'losehp(tmp, "wand"', 'tmp', 'spell'),
    ('piercer', 'hack.c', 'dmg = d(4, 6);', 'mdamageu(mtmp, dmg);', 'dmg', 'physical'),
    ('swallowed', 'mon.c', 'tmp = Maybe_Half_Phys(tmp);', 'losehp(tmp, svk.killer.name', 'tmp', 'physical'),
    ('bag', 'pickup.c', 'tmp = rnd(10);', 'losehp(tmp, "carnivorous bag"', 'tmp', 'physical'),
    ('poison', 'attrib.c', 'loss = thrown_weapon ? rnd(6) : rn1(10, 6);',
     'losehp(loss, pkiller, kprefix);', 'loss', 'gas'),
]
for name, file, start, end, var, protection in paths:
    source = (repo/'src'/file).read_text(encoding='utf8')
    body = between(source, start, end)
    if name == 'swallowed':
        body = start + body
    assert body.count('u.mith_timers[MITH_VAUL]') == 1, name
    stripped = re.sub(r'(?m)^ +if \(u.mith_timers\[MITH_VAUL\]\)\n +'+var+r' = \('+var+r' \+ 1\) / 2;\n', '', body)
    old = subprocess.check_output(['git','show',
        '5ce8b8193e4c581dd293ccac2bd0cafb4da89e96:src/'+file],cwd=repo).decode('utf8')
    if name in ('bag', 'swallowed'):
        assert 'losehp(Maybe_Half_Phys(tmp),' in old
        assert stripped.strip() == 'tmp = Maybe_Half_Phys(tmp);'
    else:
        assert stripped == between(old, start, end), (name, 'native damage changed')
    parts.append('static int vaul_'+name+'(int '+var+') {'+body+'return '+var+';}')
curse_source = (repo/'src/sit.c').read_text(encoding='utf8')
curse = re.search(r'cnt = rnd\((6 / \(\(!!Antimagic\)[\s\S]*?)\);',curse_source)[1]
curse_donor = re.search(r'cnt = rnd\((6/\(\(!!Antimagic\)[\s\S]*?)\);',upstream('sit.c'))[1]
curse_donor = curse_donor.replace('u.uvaul_duration','u.mith_timers[MITH_VAUL]')
assert re.sub(r'\s+', '', curse) == re.sub(r'\s+', '', curse_donor)
parts.append('static int vaul_curse_bound(void) { return '+curse+';}')
spells = (repo/'src/mcastu.c').read_text(encoding='utf8')
baseline = subprocess.check_output(['git','show',
    '5ce8b8193e4c581dd293ccac2bd0cafb4da89e96:src/mcastu.c'],cwd=repo).decode('utf8')
rerolled=['mcast_weaken_you','mcast_stun_you','mcast_geyser','mcast_fire_pillar',
          'mcast_lightning','mcast_paralyze','mcast_confuse_you']
for name in rerolled:
    pattern=r'(?m)^staticfn (?:void|int)\n'+name+r'\([\s\S]*?^\}'
    actual=re.search(pattern,spells)[0]
    old=re.search(pattern,baseline)[0]
    stripped=re.sub(r'(?m)^ +if \(u.mith_timers\[MITH_VAUL\]\)\n +dmg = \(dmg \+ 1\) / 2;\n','',actual)
    assert stripped==old,(name,'native helper changed beyond Vaul guard')
    modifier=re.search(r'(?m)^ +if \(Half_(?:spell|physical)_damage\)\n +dmg = \(dmg \+ 1\) / 2;\n +if \(u.mith_timers\[MITH_VAUL\]\)\n +dmg = \(dmg \+ 1\) / 2;',actual)[0]
    parts.append('static int vaul_'+name+'(int dmg) {'+modifier+'return dmg;}')
parts.append('static int (*rerolled_spells[])(int)={'+','.join('vaul_'+n for n in rerolled)+'};')
blind=re.search(r'(?m)^staticfn void\nmcast_blind_you\([\s\S]*?^\}',spells)[0]
assert blind.count('u.mith_timers[MITH_VAUL]')==1
assert 'make_blinded(duration, FALSE)' in blind
blind_init=re.search(r'long duration = Half_spell_damage \? 100L : 200L;',blind)[0]
blind_modifier=re.search(r'if \(u.mith_timers\[MITH_VAUL\]\)\n +duration = \(duration \+ 1L\) / 2L;',blind)[0]
parts.append('static long vaul_blind(void) {'+blind_init+blind_modifier+'return duration;}')
local_ray = between(zap, 'breath attacks do full damage */', 'losehp(dam, kbuf, KILLED_BY_AN);')
donor_ray = 'if (Half_spell_damage' + between(upstream('zap.c'),
    'if (Half_spell_damage && dam &&', 'losehp(dam, fltxt, KILLED_BY_AN);')
# Reattach the consumed condition and map donor breath classification only.
donor_ray = donor_ray.replace('if (Half_spell_damage\n', 'if (Half_spell_damage && dam &&\n', 1)
donor_ray = donor_ray.replace('(olet != FOOD_CLASS)', '(abstyp < 20)')
local_blast = 'if (Invulnerable)' + between(blast, 'if (Invulnerable)', 'if (adtyp == AD_FIRE) {')
donor_blast = 'if (Invulnerable)' + between(upstream('explode.c'),
    'if (Invulnerable)', 'if (adtyp == AD_FIRE) (void) burnarmor')
# Native NetHack also halves acid-explosion damage; preserve that native rule.
donor_blast = donor_blast.replace('adtyp == AD_PHYS', '(adtyp == AD_PHYS || adtyp == AD_ACID)')
local_rust = between(trap, 'You("are covered with rust!");',
                     'losehp(dam, "rusting away", KILLED_BY);')
donor_rust = between(upstream('trap.c'), 'You("are covered with rust!");',
                     'losehp(dam, "rusting away", KILLED_BY);')
for name, body, arg in [('local_ray', local_ray, 'dam'), ('donor_ray', donor_ray, 'dam'),
                        ('local_blast', local_blast, 'damu'), ('donor_blast', donor_blast, 'damu'),
                        ('local_rust', local_rust, 'dam'), ('donor_rust', donor_rust, 'dam')]:
    body = body.replace('u.uvaul_duration', 'u.mith_timers[MITH_VAUL]')
    parts.append('static int '+name+'(int '+arg+') {'+body+'return '+arg+';}')
(out/'step9c_vaul.h').write_text('\n'.join(parts), encoding='utf8')
subprocess.run(['cl', '/nologo', '/std:c11', '/W4', '/D_CRT_SECURE_NO_WARNINGS',
                '/DWIN32', '/DWIN32CON', '/I'+str(repo/'include'),
                '/I'+str(repo/'submodules/lua'), '/I'+str(out),
                '/Fe:'+str(out/'step9c_vaul.exe'), str(repo/'test/test_step9c_vaul.c')],
               cwd=out, check=True)
subprocess.run([str(out/'step9c_vaul.exe')], cwd=out, check=True)
print('PASS seven rerolled native spell helpers differ only by one scoped Vaul reduction')
