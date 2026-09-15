"""VS developer shell: OUTPUT_DIRECTORY RELEASE_DIRECTORY.

Rebuild/run Step 6/7/8/9A focused tests and depth/ledger/recovery contracts.
"""
from pathlib import Path
import subprocess
import sys

root=Path(__file__).resolve().parents[1]
out=Path(sys.argv[1]).resolve()
release=Path(sys.argv[2]).resolve()
out.mkdir(parents=True)
for script,folder in [('run_step8a.py','prior'),('run_step9a.py','sheol'),
                      ('run_depth_range.py','depth')]:
    subprocess.run([sys.executable,'-B',str(root/'test'/script),str(out/folder)],check=True)
subprocess.run(['powershell','-NoProfile','-ExecutionPolicy','Bypass',
 '-File',str(root/'test/run_ledger_runtime.ps1'),
 '-OutputDirectory',str(out/'ledger'),'-RecoverExecutable',str(release/'recover.exe')],check=True)
for script in ['test_step7_source.py','test_step8a_source.py']:
    subprocess.run([sys.executable,'-B',str(root/'test'/script),str(release/'nhdat500')],check=True)
print('PASS cumulative focused/source/depth/ledger/recovery:',release,flush=True)
