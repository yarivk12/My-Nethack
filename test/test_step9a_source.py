"""Pinned donor table fidelity, source boundaries and current DLB bytes.

python test_step9a_source.py DONOR_CLONE [nhdat500 ...]
"""
from pathlib import Path
import io
import re
import subprocess
import sys
from step9c_source_projection import project
from test_step10b_source import project as step10b_project

repo=Path(__file__).resolve().parents[1]
pin='439b8d63d3d1ca78fb08588dd43f61874114b21a'
base='5ce8b8193e4c581dd293ccac2bd0cafb4da89e96'
clone=Path(sys.argv[1])
def read(p):return (repo/p).read_text(encoding='utf8')
def donor(p):return subprocess.check_output(['git','show',pin+':'+p],cwd=clone).decode().replace('\r\n','\n')
def old(p):return subprocess.check_output(['git','show',base+':'+p],cwd=repo).decode().replace('\r\n','\n')
def block(source,start):
    pos=source.index(start);begin=source.index('(',pos);n=0;quoted=False
    for i in range(begin,len(source)):
        c=source[i]
        if c=='"' and source[i-1]!='\\':quoted=not quoted
        if quoted:continue
        if c=='(':n+=1
        elif c==')':
            n-=1
            if not n:return source[pos:i+1]
    raise AssertionError(start)
names=['arctic fern spore','evil eye','chillbug','dark Angel','weeping angel',
 'weeping archangel','arctic fern sprout','arctic fern','white naga hatchling',
 'white naga','blue slime','ice golem','crystal ice golem','Executioner','Punisher']
source=donor('src/monst.c')
for name in names:
    expected=block(source,'MON("'+name+'",')
    expected=expected.replace('MON("'+name+'",','MON(NAM("'+name+'"),',1)
    expected=re.sub(r'SIZ\(([^,]+),\s*([^,]+),\s*(?:0|sizeof\(struct epri\)),',r'SIZ(\1, \2,',expected)
    if name=='weeping angel':expected=expected.replace('(G_NOCORPSE|','(G_SHEOL|G_NOCORPSE|',1)
    actual=block(read('include/monsters.h'),'MON(NAM("'+name+'"),')
    actual=re.sub(r',\s*\d+,\s*(CLR_\w+|HI_\w+),\s*[A-Z_]+\)$',r', \1)',actual)
    assert re.sub(r'\s','',actual)==re.sub(r'\s','',expected),name
print('PASS all 15 complete donor monster definitions; only documented naming/ABI/generation adaptation')
# Checkpoint publication updates the cumulative README, preserving Steps 5–8.
# Keep the gameplay boundary checks independent of the old uncommitted status.
for heading in ['Step 5: shop optimization', 'Step 6: NerfHack dungeon enrichment',
                'Step 7: classic NerfHack Lost Tomb', 'Step 8: Ruins of Moria']:
    pattern=r'(?ms)^## '+re.escape(heading)+r'\n.*?(?=^## |\Z)'
    assert re.search(pattern, read('README.md'))[0] == re.search(pattern, old('README.md'))[0], heading
for p in ['include/global.h','include/dungeon.h','include/you.h',
          'include/monst.h','src/save.c','src/restore.c','src/bones.c',
          'src/files.c','util/recover.c','include/artilist.h',
          'src/mkroom.c','src/shknam.c']:
    assert project(p, step10b_project(p, read(p)))==old(p),p
def strip_step10b3_artifact_changes(text):
    """Project current B3 artifact additions out of the historical check."""
    for fragment in (
        'staticfn int invoke_silver_key_portal(struct obj *) NONNULLARG1;\n',
        'staticfn int invoke_altmode(struct obj *) NONNULLARG1;\n',
        '''                switch (m) {
                case ART_MIRROR_BRAND:
                    otmp->obranch_material = mod ? SILVER : 0;
                    break;
                case ART_SANSARA_MIRROR:
                    otmp->obranch_material = mod ? GOLD : 0;
                    break;
                case ART_SOULMIRROR:
                    otmp->obranch_material = mod ? MITHRIL : 0;
                    break;
                case ART_SILVER_KEY:
                    otmp->obranch_material = mod ? SILVER : 0;
                    break;
                default:
                    break;
                }
''',
        '''/* Soulmirror's pinned +7 is artifact-specific; ordinary PLATE_MAIL keeps its
   normal native armor value. */
int
artifact_arm_bonus(struct obj *obj)
{
    return obj && is_art(obj, ART_SOULMIRROR) ? 7 : 0;
}

''',
        '''    if (spfx & SPFX_PCTRL) {
        if (on)
            EPolymorph_control |= wp_mask;
        else
            EPolymorph_control &= ~wp_mask;
    }
''',
        '''    if (spfx & SPFX_DISPL) {
        if (on)
            EDisplaced |= wp_mask;
        else
            EDisplaced &= ~wp_mask;
    }
''',
        '    int dnum = 1, dsize = 4;\n'
        '    boolean mirror_brand = is_art(mb, ART_MIRROR_BRAND);\n',
        '    if (mirror_brand)\n        dnum = 2, dsize = 10;\n',
        '    if ((spfx & SPFX_REFLECT) && (wp_mask & (W_WEP | W_ARMOR))) {',
        '    if ((spfx & SPFX_REFLECT) && (wp_mask & W_WEP)) {',
    ):
        if fragment == '    if ((spfx & SPFX_REFLECT) && (wp_mask & W_WEP)) {':
            continue
        assert text.count(fragment) == 1, fragment[:50]
        text = text.replace(fragment, '' if 'REFLECT' not in fragment else
                            '    if ((spfx & SPFX_REFLECT) && (wp_mask & W_WEP)) {')

    damage = '    *dmgptr += mirror_brand ? d(dnum, dsize) : rnd(4);'
    assert text.count(damage) == 4
    text = text.replace(damage, '    *dmgptr += rnd(4);')
    comments = [' /* (2..3)d4 */', ' /* (3..4)d4 */',
                ' /* (3..5)d4 */', ' /* (4..6)d4 */']
    lines = text.splitlines(keepends=True)
    for index, line in enumerate(lines):
        if line.lstrip() == '*dmgptr += rnd(4);\n':
            lines[index] = line[:-1] + comments.pop(0) + '\n'
    assert not comments
    text = ''.join(lines)

    infinity = '''    /* The local representation of the donor's second beam is a bounded
       artifact-only damage increment when its alternate mode is active. */
    if (is_art(otmp, ART_INFINITY_S_MIRRORED_ARC) && otmp->usecount)
        *dmgptr += d(3, 3);
'''
    assert text.count(infinity) == 1
    text = text.replace(infinity, '')

    portal_start = text.index('boolean\nsilver_key_destination_valid')
    portal_end = text.index('staticfn int\ninvoke_create_portal', portal_start)
    text = text[:portal_start] + text[portal_end:]

    dispatch = '''    /* Readability is the Necronomicon's authoritative interface.  Neither
     * this harmless direction nor the dormant Silver Key path uses cooldown. */
    if (is_art(obj, ART_NECRONOMICON)) {
        pline("The Necronomicon must be read, not invoked.");
        return ECMD_TIME;
    }
    if (is_art(obj, ART_SILVER_KEY))
        return invoke_silver_key_portal(obj);

'''
    assert text.count(dispatch) == 1
    text = text.replace(dispatch, '')
    assert text.count('        case ALTMODE: res = invoke_altmode(obj); break;\n') == 1
    text = text.replace('        case ALTMODE: res = invoke_altmode(obj); break;\n', '')

    alt_start = text.index('staticfn int\ninvoke_altmode')
    alt_end = text.index('/* will freeing this object', alt_start)
    text = text[:alt_start] + text[alt_end:]
    assert text.count('        { &EPolymorph_control, SPFX_PCTRL },\n') == 1
    text = text.replace('        { &EPolymorph_control, SPFX_PCTRL },\n', '')
    return text


artifact=strip_step10b3_artifact_changes(read('src/artifact.c'))
for ident in ['GLOWING_DRAGON_SCALE_MAIL','GLOWING_DRAGON_SCALES']:
    artifact=re.sub(r'\s+\|\| obj->otyp == '+ident+r'\b','',artifact)
assert artifact==old('src/artifact.c'),'artifact changes beyond Step9B worn glowing armor light'
assert '#define EDITLEVEL 5' in read('include/patchlevel.h') # Step 10 ID epoch
assert len(re.findall(r'name\s*=\s*"Sheol"',read('dat/dungeon.lua')))==2
dungeon = read('dat/dungeon.lua')
assert 'name="Sheol", base=30, range=170, direction="down"' in dungeon
assert 'name="The Dragon Caves", base=30, range=170, direction="down"' in dungeon
assert 'name="Mithardir", base=30, range=170, branchtype="portal"' in dungeon
assert 'base = 6, range = 2' in read('dat/dungeon.lua')
schedule = read('src/dungeon.c')
assert 'Step9 parent branches use the shared persistent scheduler' in schedule
assert 'for (dlevel = 108; dlevel <= 110; ++dlevel)' not in schedule
assert 'step6b_rebase_branch(sheol)' in schedule
assert 'step6b_rebase_branch(dragon_caves)' in schedule
assert 'step6b_rebase_branch(mithardir)' in schedule
assert 'base = 107' not in read('dat/dungeon.lua')
assert not (repo/'dat/tomb-2.lua').exists()
assert 'MONSPELL(PUNISHMENT,' in read('include/mcastu.h')
assert 'if (is_weeping(mdat) && canseemon(mon))' in read('src/mon.c')
assert 'case CRYSTAL_PICK:' in read('src/apply.c')
assert '#define Frozen_feet (u.uspare1)' in read('include/youprop.h')
assert '#define mon_frozen_feet(mon) ((mon)->mspare1 & 31L)' in read('include/youprop.h')
assert '#define set_mon_frozen_feet(mon, value)' in read('include/youprop.h')
print('PASS README/save/structural/shop/endgame boundaries, one 6..8-level Sheol, randomized Step9 parents and compatibility epoch')
# These map-data spaces must survive publication; outside the actual map
# literals, retain the whitespace check relaxed by the path-specific Git rule.
for name in ['cat1', 'cat3', 'chalv2', 'drgnA', 'drgnB', 'drgnC', 'drgnD',
             'palace_e', 'palace_f']:
    source=read('dat/'+name+'.lua')
    code,count=re.subn(r'map\s*=\s*\[=\[[\s\S]*?\]=\]', 'map=""', source)
    assert count>0, name
    assert all(line==line.rstrip(' \t') for line in code.splitlines()), name
print('PASS intentional ASCII map padding; no trailing whitespace outside map literals')
resources=['sheolfil.lua','sheolmid.lua','palace_f.lua','palace_e.lua']
for name in resources:
    assert pin in read('dat/'+name)
    for manifest in ['sys/unix/Makefile.top','sys/windows/Makefile.nmake','sys/windows/vs/files.props']:
        assert name in read(manifest),(name,manifest)
for path in sys.argv[2:]:
    data=Path(path).read_bytes();f=io.BytesIO(data)
    revision,count,_,_,total=map(int,f.readline().split())
    assert revision==1 and total==len(data)
    entries=[]
    for _ in range(count):
        name,offset=f.readline().split();assert name[:1]==b'n'
        entries.append((name[1:].decode(),int(offset)))
    for name in resources+['dungeon.lua']:
        indexes=[i for i,(n,_) in enumerate(entries) if n==name]
        assert len(indexes)==1,(path,name)
        i=indexes[0];end=entries[i+1][1] if i+1<len(entries) else total
        assert data[entries[i][1]:end].decode().replace('\r\n','\n')==read('dat/'+name),(path,name,'stale')
    print('PASS all Sheol resources and dungeon bytes packaged:',path)
