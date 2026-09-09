"""Pinned donor table fidelity, source boundaries and current DLB bytes.

python test_step9a_source.py DONOR_CLONE [nhdat500 ...]
"""
from pathlib import Path
import io
import re
import subprocess
import sys
from step9c_source_projection import project

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
    assert project(p, read(p))==old(p),p
artifact=read('src/artifact.c')
for ident in ['GLOWING_DRAGON_SCALE_MAIL','GLOWING_DRAGON_SCALES']:
    artifact=re.sub(r'\s+\|\| obj->otyp == '+ident+r'\b','',artifact)
assert artifact==old('src/artifact.c'),'artifact changes beyond Step9B worn glowing armor light'
assert '#define EDITLEVEL 4' in read('include/patchlevel.h')
assert len(re.findall(r'name\s*=\s*"Sheol"',read('dat/dungeon.lua')))==2
assert 'name="Sheol", base=108, direction="down"' in read('dat/dungeon.lua')
assert 'base = 6, range = 2' in read('dat/dungeon.lua')
assert 'for (dlevel = 108; dlevel <= 110; ++dlevel)\n        used[dlevel] = TRUE;' in read('src/dungeon.c')
assert 'base = 107' not in read('dat/dungeon.lua')
assert not (repo/'dat/tomb-2.lua').exists()
assert 'MONSPELL(PUNISHMENT,' in read('include/mcastu.h')
assert 'if (is_weeping(mdat) && canseemon(mon))' in read('src/mon.c')
assert 'case CRYSTAL_PICK:' in read('src/apply.c')
assert '#define Frozen_feet (u.uspare1)' in read('include/youprop.h')
assert '#define mon_frozen_feet(mon) ((mon)->mspare1 & 31L)' in read('include/youprop.h')
assert '#define set_mon_frozen_feet(mon, value)' in read('include/youprop.h')
print('PASS README/save/structural/shop/endgame boundaries, one 6..8-level Sheol, temporary reservations and compatibility epoch')
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
