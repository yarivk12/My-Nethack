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
    assert re.findall(r'Portal to Mithardir: (\d+)',text)==['110'],text
    assert re.findall(r'Mithardir: levels (\d+) to (\d+)',text)==[('110','119')],text
    assert re.findall(r'chalv2: (\d+)',text)==['110'],text
    for name,depth in [('ossa1',110),('mith1',111),('mith2',112),('mith3',113),
                       ('cat1',114),('cat2',115),('cat3',116)]:
        assert re.findall(r'\b'+name+r': (\d+)',text)==[str(depth)],(name,text)
    print('PASS exactly one ten-level Mithardir via DoD110 portal; seven fixed maps and three generated floors:',directory.name)
