"""Pinned Dragon Caves database fidelity, native boundaries, DLB byte checks.

Usage: python -B test/test_step9b_source.py UNNETHACK_CLONE [nhdat500 ...]
"""
from pathlib import Path
import io
import re
import runpy
import sys
from step9c_source_projection import project

# Reuse the complete preceding boundary gate and its pinned source reader.
prior=runpy.run_path(str(Path(__file__).with_name('test_step9a_source.py')))
read,old,donor,block=(prior[n] for n in ('read','old','donor','block'))
for name,local in [('baby glowing dragon','baby glowing dragon'),
                   ('glowing dragon','glowing dragon'),
                   ('chromatic dragon','chromatic cave dragon')]:
    expected=block(donor('src/monst.c'),'MON("'+name+'",')
    expected=expected.replace('MON("'+name+'",','MON(NAM("'+local+'"),',1)
    expected=re.sub(r'SIZ\(([^,]+),\s*([^,]+),\s*0,',r'SIZ(\1, \2,',expected)
    if name=='glowing dragon':
        expected=expected.replace('(G_GENO|1)','(G_NOGEN|G_GENO|1)',1)
    actual=block(read('include/monsters.h'),'MON(NAM("'+local+'"),')
    actual=re.sub(r',\s*\d+,\s*(CLR_\w+|HI_\w+),\s*[A-Z_]+\)$',r', \1)',actual)
    assert re.sub(r'\s','',actual)==re.sub(r'\s','',expected),name
assert block(read('include/monsters.h'),'MON(NAM("Chromatic Dragon"),') == \
       block(old('include/monsters.h'),'MON(NAM("Chromatic Dragon"),')
assert read('dat/Cav-goal.lua')==old('dat/Cav-goal.lua')
# Freeze the accepted cumulative armor conversion/reading behavior.
assert project('src/read.c', read('src/read.c'))==project('src/read.c', prior['checkpoint']('src/read.c'))
assert 'name="The Dragon Caves", base=30, range=170, direction="down"' in read('dat/dungeon.lua')
assert 'AD_LAVA' in read('include/monattk.h')
assert 'dragon_revivals' in read('include/youprop.h')
source=read('src/dungeon.c')
assert source.index('nlevels > LEV_LIMIT - pd->n_levs') < source.index('tmpl = &pd->tmplevel[pd->n_levs + f]')
assert source.index('nbranches > BRANCH_LIMIT - pd->n_brs') < source.index('tmpb = &(pd->tmpbranch[pd->n_brs + f])')
print('PASS three complete pinned dragon definitions, native quest/armor conversion boundaries and pre-write loader checks')
resources=['drgnA.lua','drgnB.lua','drgnC.lua','drgnD.lua']
for name in resources:
    assert prior['pin'] in read('dat/'+name)
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
    for name in resources:
        indexes=[i for i,(n,_) in enumerate(entries) if n==name]
        assert len(indexes)==1,(path,name)
        i=indexes[0];end=entries[i+1][1] if i+1<len(entries) else total
        assert data[entries[i][1]:end].decode().replace('\r\n','\n')==read('dat/'+name),(path,name,'stale')
    print('PASS all four Dragon Caves resources match packaged bytes:',path)
