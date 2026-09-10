"""Fresh packaged Mithardir topology plus cumulative Step 6/7/8/9A/9B rules."""
from pathlib import Path
import re
import subprocess
import sys

release, output = sys.argv[1:3]
count = sys.argv[3] if len(sys.argv)>3 else '8'
subprocess.run([sys.executable, '-B', str(Path(__file__).with_name('run_step9b_topology.py')),
                release, output, count], check=True)
for directory in Path(output).iterdir():
    if not directory.is_dir():
        continue
    text=(directory/'topology.txt').read_text()
    parent=int(re.findall(r'Portal to Mithardir: (\d+)',text)[0])
    assert 30<=parent<=199
    assert re.findall(r'Mithardir: levels (\d+) to (\d+)',text)==[
        (str(parent),str(parent+9))],text
    assert re.findall(r'chalv2: (\d+)',text)==[str(parent)],text
    for offset,name in enumerate(['ossa1','mith1','mith2','mith3',
                                  'cat1','cat2','cat3']):
        depth=parent+offset
        assert re.findall(r'\b'+name+r': (\d+)',text)==[str(depth)],(name,text)
    print('PASS exactly one ten-level Mithardir via randomized portal parent',
          parent,'with seven fixed maps and three generated floors:',directory.name)
