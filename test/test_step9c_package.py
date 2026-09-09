"""Validate every Mithardir Lua resource in final Windows DLB archives."""
from pathlib import Path
import io,sys
repo=Path(__file__).resolve().parents[1]
names=['chalv2','ossa1','mith1','mith2','mith3','cat1','cat2','cat3']
pin='17bc64f77ac566e7b90c0c6c6652e2a5f3a995c0'
for name in names:
    assert pin in (repo/'dat'/f'{name}.lua').read_text(encoding='utf8'),name
    for manifest in ['sys/unix/Makefile.top','sys/windows/Makefile.nmake','sys/windows/vs/files.props']:
        assert name+'.lua' in (repo/manifest).read_text(encoding='utf8'),(manifest,name)
for argument in sys.argv[1:]:
    path=Path(argument);data=path.read_bytes();stream=io.BytesIO(data)
    revision,count,_,_,total=map(int,stream.readline().split())
    assert revision==1 and total==len(data)
    entries=[]
    for _ in range(count):
        name,offset=stream.readline().split();assert name[:1]==b'n'
        entries.append((name[1:].decode(),int(offset)))
    for name in [n+'.lua' for n in names]+['dungeon.lua','quest.lua']:
        matches=[i for i,(n,_) in enumerate(entries) if n==name]
        assert len(matches)==1,(path,name,matches)
        i=matches[0];end=entries[i+1][1] if i+1<len(entries) else total
        actual=data[entries[i][1]:end].decode().replace('\r\n','\n')
        assert actual==(repo/'dat'/name).read_text(encoding='utf8'),(path,name,'stale bytes')
    print('PASS eight Mithardir resources plus dungeon/quest bytes, pinned provenance and all three manifests:',path)
