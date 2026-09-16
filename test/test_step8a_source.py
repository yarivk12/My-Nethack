"""Step 8A source, version, donor-conversion and DLB freshness contract."""
from pathlib import Path
import io
import re
import subprocess
import sys
from test_step10b_source import project as step10b_project

repo=Path(__file__).resolve().parents[1]
baseline='4ab8e05f7a48833f303f11036d3697737b9946a0'
pin='439b8d63d3d1ca78fb08588dd43f61874114b21a'
names=['moria1-1','moria2-1','moria3-1','moria4-1','moria4-2','moria4-3',
       'moria4-4','moria5-1','moria6-1','moria6-2']
def read(path): return (repo/path).read_text(encoding='utf8')
def old(path):
    return subprocess.check_output(['git','show',baseline+':'+path],cwd=repo).decode().replace('\r\n','\n')

for p in ['include/global.h','include/dungeon.h','src/save.c',
          'src/restore.c','src/bones.c','src/files.c','util/recover.c']:
    assert step10b_project(p, read(p))==old(p),p
assert '## Step 8: Ruins of Moria' in read('README.md')
assert '439b8d63d3d1ca78fb08588dd43f61874114b21a' in read('README.md')
assert '#define EDITLEVEL 6' in read('include/patchlevel.h') # Step 10 ID epoch
dungeon=read('dat/dungeon.lua')
assert dungeon.count('name = "The Ruins of Moria"')==2
assert re.search(r'name = "The Ruins of Moria",\s+base = 30,\s+range = 170,\s+direction = "up"',dungeon)
assert 'base = 107' not in dungeon
assert 'name = "moria6-" .. math.random(1,2)' in dungeon
assert 'name = "moria4-" .. math.random(1,4)' in dungeon
assert 'base = 6' in dungeon and 'entry = -1' in dungeon
assert 'base = 106' not in dungeon
for name in names:
    assert pin in read('dat/'+name+'.lua')
    for manifest in ['sys/unix/Makefile.top','sys/windows/Makefile.nmake','sys/windows/vs/files.props']:
        assert name+'.lua' in read(manifest) or 'moria?-?.lua' in read(manifest),(name,manifest)
assert set(p.stem for p in (repo/'dat').glob('moria*.lua'))==set(names)
assert 'selection.circle' in read('dat/moria1-1.lua')
assert ' | selection.' in read('dat/moria1-1.lua')
assert 'MORIA_PORTAL' in read('include/artilist.h')
assert 'portal->dst.dnum = portal->dst.dlevel = -1' in read('src/artifact.c')
assert read('src/do.c').index('moria_keep_level())') < read('src/do.c').index('delete_levelfile(l_idx);\n        svl.level_info[l_idx].flags = 0;')
assert 'migrate_to_level(mtmp, ledger_no(&dest), MIGR_RANDOM' in read('src/do.c')
assert 'if (!mtmp || is_swamp_fern(mtmp->data)\n                    || is_arctic_fern(mtmp->data))' in read('src/explode.c')
assert '&& !is_swamp_fern(mdef->data) && !is_arctic_fern(mdef->data))\n        return ALLOW_M | ALLOW_TM;' in read('src/mon.c')
assert 'sobj_at(IRON_SAFE, u.ux + u.dx, u.uy + u.dy));' in read('src/apply.c')
assert 'case AD_PHYS: /* Moria spores:' in read('src/mhitu.c')
assert 'mdamageu(mtmp, Maybe_Half_Phys(tmp));' in read('src/mhitu.c')
grave=read('src/dig.c')
assert grave.index('int moria_grave = 0;') < grave.index('dig_up_grave(cc, moria_grave);')
assert 'dig_up_grave(coord *cc, int moria_grave)' in grave
assert 'Missing Ruins of Moria branch' in read('src/dungeon.c')
assert 'moria->end1.dlevel = (xint16) dlevel' in read('src/dungeon.c')
assert 'step6b_pick_depth(used, FALSE)' in read('src/dungeon.c')
print('PASS source: randomized Moria scheduler, ten pinned resources, variant weights, unchanged save structures/recovery')

for path in sys.argv[1:]:
    data=Path(path).read_bytes(); f=io.BytesIO(data)
    revision,count,_,_,total=map(int,f.readline().split())
    assert revision==1 and total==len(data)
    entries=[]
    for _ in range(count):
        name,offset=f.readline().split();assert name[:1]==b'n'
        entries.append((name[1:].decode(),int(offset)))
    assert not any('tomb-2' in n for n,_ in entries)
    for name in [n+'.lua' for n in names]+['dungeon.lua']:
        ix=[i for i,(n,_) in enumerate(entries) if n==name]
        assert len(ix)==1,(path,name)
        i=ix[0]; end=entries[i+1][1] if i+1<len(entries) else total
        assert data[entries[i][1]:end].decode().replace('\r\n','\n')==read('dat/'+name),(path,name,'stale')
    print('PASS DLB: all ten Moria resources and dungeon bytes current; classic Tomb only:',path)
