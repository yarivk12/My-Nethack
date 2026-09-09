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
    assert re.findall(r'Stair to The Dragon Caves: (\d+)',text)==['109'],text
    assert re.findall(r'The Dragon Caves: levels (\d+) to (\d+)',text)==[('110','113')],text
    assert re.findall(r'drgn([ABCD]): (\d+)',text)==list(zip('ABCD',map(str,range(110,114)))),text
    print('PASS exactly one four-level descending Dragon Caves at DoD109:',directory.name)
