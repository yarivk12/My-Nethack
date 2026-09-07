"""Fresh packaged topology; includes the unchanged Step 7 runner gates."""
from pathlib import Path
import json
import re
import subprocess
import sys

release, output = sys.argv[1:3]
count=sys.argv[3] if len(sys.argv)>3 else '8'
subprocess.run([sys.executable,str(Path(__file__).with_name('run_step7_topology.py')),
                release,output,count],check=True)
for game in Path(output).iterdir():
    if not game.is_dir(): continue
    text=(game/'topology.txt').read_text()
    entrances = re.findall(r'Stair to The Ruins of Moria: (\d+)',text)
    assert len(entrances) == 1
    entrance = int(entrances[0])
    assert 30 <= entrance <= 199
    assert re.search(r'The Ruins of Moria: levels %d to %d, entrance from below' %
                     (entrance-6, entrance-1), text)
    expected={n: entrance-n for n in range(1,7)}
    maps=re.findall(r'moria([1-6])-([1-4]): (\d+)',text)
    assert len(maps)==6
    assert len({n for n,_,_ in maps})==6
    for n,v,depth in maps:
        assert int(depth)==expected[int(n)]
        assert int(v)<=({4:4,6:2}.get(int(n),1))
    reserved=[int(n) for n in re.findall(r'(?:bigrm|x6b-giant|x6b-realzoo|x6b-dragon): (\d+)',text)]
    reserved += [int(n) for n in re.findall(r'Stair to The (?:Lost Tomb|Temple of Moloch): (\d+)',text)]
    assert entrance not in reserved
    print('PASS packaged Moria topology',game.name,maps)
print('PASS exactly one upward six-level Moria at randomized DoD30-199; prior branches remain random and collision-free')
