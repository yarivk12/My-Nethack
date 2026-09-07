"""Source/packaged-data regression contracts; run from any directory.

python test/test_step7_source.py [binary/Release/x64/nhdat500 ...]
"""
from pathlib import Path
import io
import re
import subprocess
import sys

repo=Path(__file__).resolve().parents[1]
base="9191de7079624e94acdde19d07814a0f03ce9450"
def old(path):
    return subprocess.check_output(["git","show",base+":"+path],cwd=repo).decode().replace("\r\n","\n")
def now(path):
    return (repo/path).read_text(encoding="utf-8")
for path in ["src/mklev.c","src/mkroom.c","src/shknam.c","include/dungeon.h",
             "include/global.h","src/save.c","src/restore.c","src/bones.c",
             "dat/bigrm-14.lua"]:
    assert now(path)==old(path),path
def function(text,name):
    matches=re.findall(r"(?m)^(?:staticfn )?(?:const )?\w+(?: \*)?\n"
                       +name+r"\([\s\S]*?^\}",text)
    assert len(matches)==1,name
    return matches[0]
for path,name in [("src/apply.c","dorub"),("src/potion.c","djinni_from_bottle"),
                  ("src/zap.c","makewish")]:
    assert function(now(path),name)==function(old(path),name),name
for path,pattern,expected in [
    ("include/monsters.h",r'MON\(NAM\("([^"\n]+)"\)',{"shadow"}),
    ("include/objects.h",r'TOOL\("([^"\n]+)"',{"magic candle"})]:
    before=set(re.findall(pattern,old(path))); after=set(re.findall(pattern,now(path)))
    assert after-before==expected and before<=after,path
assert not (repo/"dat/tomb-2.lua").exists()
dungeon=now("dat/dungeon.lua")
assert dungeon.count('name = "The Lost Tomb"')==2
assert 'levels = { { name = "tomb-1", bonetag = "Z", base = 1 } }' in dungeon
assert 'tomb-2' not in dungeon
assert 'base = 200' in dungeon
for path in ["sys/unix/Makefile.top","sys/windows/Makefile.nmake","sys/windows/vs/files.props"]:
    assert "tomb-1.lua" in now(path) and "tomb-2" not in now(path)
assert "#define EDITLEVEL 2" in now("include/patchlevel.h")
print("PASS source: protected Step 5/6, vanilla wishing/lamp rub, only Shadow/Magic Candle added, classic one-level Tomb/manifests, save epoch")

for arg in sys.argv[1:]:
    data=Path(arg).read_bytes(); stream=io.BytesIO(data)
    revision,count,_,_,total=map(int,stream.readline().split())
    assert revision==1 and total==len(data)
    entries=[]
    for _ in range(count):
        name,offset=stream.readline().split()
        assert name[:1]==b'n'
        entries.append((name[1:].decode(),int(offset)))
    assert not any("tomb-2" in name for name,_ in entries)
    for name in ["tomb-1.lua","moloch.lua","bigrm-14.lua","dungeon.lua"]:
        indices=[i for i,(n,_) in enumerate(entries) if n==name]
        assert len(indices)==1,(arg,name)
        i=indices[0]; end=entries[i+1][1] if i+1<len(entries) else total
        actual=data[entries[i][1]:end].replace(b"\r\n",b"\n")
        assert actual.decode()==now("dat/"+name),(arg,name,"stale packaged resource")
    print("PASS packaged resource bytes match final sources:",arg)
