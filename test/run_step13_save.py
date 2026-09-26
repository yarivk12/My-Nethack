"""Actual tty save/restore and recover.exe, including enhanced ownership chains."""
from pathlib import Path
import os,subprocess,sys
from run_step8a_runtime import Game
release,output=sys.argv[1:3]
os.environ['STEP13_GAME_FIXTURE']='1'
os.environ.pop('NETHACK_STEP13_TEST',None)
g=Game(release,output)
try:
 g.save()
 g=Game(release,output,restore=True)
 text=(Path(output)/'step13-game-results.txt').read_text()
 assert text.count('PASS restored all eight ownership paths')==1,text
 assert text.count('PASS restored all 23 Utility identities')==1,text
 # Quit the host process with the ordinary checkpoint intact.
finally:g.close()
r=subprocess.run([str(Path('binary/Release/x64/recover.exe').resolve()),'-d',str(Path(output).resolve()),'wizard'],capture_output=True,text=True)
(Path(output)/'recovery.log').write_text(r.stdout+r.stderr)
assert r.returncode==0,(r.returncode,r.stdout,r.stderr)
g=Game(release,output,restore=True)
try:
 text=(Path(output)/'step13-game-results.txt').read_text()
 assert text.count('PASS restored all eight ownership paths')==2,text
 assert text.count('PASS restored all 23 Utility identities')==2,text
 print(text)
 print('PASS enhanced full game save/restore and native recover.exe checkpoint restore')
finally:g.close()
