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
for directory in Path(output).iterdir():
    if not directory.is_dir():continue
    text=(directory/'topology.txt').read_text()
    assert re.findall(r'Stair to Sheol: (\d+)',text)==['108'],text
    bounds=re.findall(r'\bSheol: levels (\d+) to (\d+)',text)
    assert len(bounds)==1,text
    low,high=map(int,bounds[0]);length=high-low+1
    assert low==109 and length in (6,7,8),(low,high)
    lengths.add(length)
    assert re.search(r'sheolmid: 110\b',text)
    assert re.search(r'palace_f: '+str(high-1)+r'\b',text)
    assert re.search(r'palace_e: '+str(high)+r'\b',text)
    # Prior scheduler placements cannot occupy any Step 9 test parent.
    prior=[int(d) for d in re.findall(r'(?:bigrm|x6b-giant|x6b-realzoo|x6b-dragon): (\d+)',text)]
    prior += [int(d) for d in re.findall(r'Stair to The (?:Lost Tomb|Temple of Moloch|Ruins of Moria): (\d+)',text)]
    assert not set(prior)&{108,109,110},prior
    print('PASS Sheol DoD108,',length,'descending levels and all temporary reservations:',directory.name)
print('PASS sampled Sheol lengths:',sorted(lengths))
