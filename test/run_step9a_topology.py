"""Packaged Sheol topology and all prior occurrence/reservation rules."""
from pathlib import Path
import re
import subprocess
import sys

release,output=sys.argv[1:3]
count=sys.argv[3] if len(sys.argv)>3 else '8'
if count!='--verify-existing':
    subprocess.run([sys.executable,'-B',str(Path(__file__).with_name('run_step8a_topology.py')),
                    release,output,count],check=True)
lengths=set()
sheol_parents=set()
dragon_parents=set()
mithardir_parents=set()
for directory in Path(output).iterdir():
    if not directory.is_dir():continue
    text=(directory/'topology.txt').read_text()
    sheol_values=re.findall(r'Stair to Sheol: (\d+)',text)
    dragon_values=re.findall(r'Stair to The Dragon Caves: (\d+)',text)
    mithardir_values=re.findall(r'Portal to Mithardir: (\d+)',text)
    assert len(sheol_values)==len(dragon_values)==len(mithardir_values)==1,text
    sheol_parent,dragon_parent,mithardir_parent=map(
        int,(sheol_values[0],dragon_values[0],mithardir_values[0]))
    assert all(30<=d<=199 for d in
               (sheol_parent,dragon_parent,mithardir_parent)),text
    assert len({sheol_parent,dragon_parent,mithardir_parent})==3,text
    sheol_parents.add(sheol_parent)
    dragon_parents.add(dragon_parent)
    mithardir_parents.add(mithardir_parent)
    bounds=re.findall(r'\bSheol: levels (\d+) to (\d+)',text)
    assert len(bounds)==1,text
    low,high=map(int,bounds[0]);length=high-low+1
    assert low==sheol_parent+1 and length in (6,7,8),(low,high)
    lengths.add(length)
    assert re.search(r'sheolmid: '+str(sheol_parent+2)+r'\b',text)
    assert re.search(r'palace_f: '+str(high-1)+r'\b',text)
    assert re.search(r'palace_e: '+str(high)+r'\b',text)
    caves=re.findall(r'The Dragon Caves: levels (\d+) to (\d+)',text)
    assert caves==[(str(dragon_parent+1),str(dragon_parent+4))],text
    assert re.findall(r'drgnA: (\d+)',text)==[str(dragon_parent+1)],text
    assert re.findall(r'drgnD: (\d+)',text)==[str(dragon_parent+4)],text
    mithardir=re.findall(r'Mithardir: levels (\d+) to (\d+)',text)
    assert mithardir==[(str(mithardir_parent),str(mithardir_parent+9))],text
    assert re.findall(r'chalv2: (\d+)',text)==[str(mithardir_parent)],text
    # Prior scheduler placements cannot occupy any Step 9 test parent.
    prior=[int(d) for d in re.findall(r'(?:bigrm|x6b-giant|x6b-realzoo|x6b-dragon): (\d+)',text)]
    prior += [int(d) for d in re.findall(r'Stair to The (?:Lost Tomb|Temple of Moloch|Ruins of Moria): (\d+)',text)]
    assert not set(prior)&{sheol_parent,dragon_parent,mithardir_parent},prior
    print('PASS randomized Step9 parents',
          (sheol_parent,dragon_parent,mithardir_parent),
          length,'descending Sheol levels:',directory.name)
assert len(sheol_parents)>1 and len(dragon_parents)>1 and len(mithardir_parents)>1
print('PASS sampled randomized Step9 parents:',
      sorted(sheol_parents),sorted(dragon_parents),sorted(mithardir_parents))
print('PASS sampled Sheol lengths:',sorted(lengths))
