"""Fresh actual wizard games: Caves plus all preceding scheduler contracts."""
from pathlib import Path
import re
import subprocess
import sys

release,output=sys.argv[1:3]
count=sys.argv[3] if len(sys.argv)>3 else '8'
subprocess.run([sys.executable,'-B',str(Path(__file__).with_name('run_step9a_topology.py')),
                release,output,count],check=True)
for directory in Path(output).iterdir():
    if not directory.is_dir():continue
    text=(directory/'topology.txt').read_text()
    parent=int(re.findall(r'Stair to The Dragon Caves: (\d+)',text)[0])
    assert 30<=parent<=199
    assert re.findall(r'The Dragon Caves: levels (\d+) to (\d+)',text)==[
        (str(parent+1),str(parent+4))],text
    assert re.findall(r'drgn([ABCD]): (\d+)',text)==[
        (name,str(parent+index)) for index,name in enumerate('ABCD',1)],text
    print('PASS exactly one randomized four-level Dragon Caves parent:',parent,directory.name)
