"""Rebuild existing depth-range tests against current production functions.

Run in a VS developer shell: python test/run_depth_range.py OUTPUT_DIRECTORY.
Extract only the tested functions, as in the ledger regression runner, to
avoid linking the whole game or reusing an older precompiled test binary.
"""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
out = Path(sys.argv[1]).resolve()
out.mkdir(parents=True, exist_ok=True)
def source(path):
    return (repo/path).read_text(encoding="utf-8")
def function(path, name):
    matches = re.findall(r"(?m)^(?:staticfn )?(?:const )?\w+(?: \*)?\n"
                         + name + r"\([\s\S]*?^\}", source(path))
    assert len(matches)==1, (path,name)
    return matches[0]

parts = ['#include "hack.h"', 'struct restore_info restoreinfo;',
         'volatile struct window_procs windowprocs;',
         'void pline(const char *fmt, ...) { (void)fmt; }']
for name in ['depth','deepest_lev_reached','ledger_no','ledger_to_dnum',
             'ledger_to_dlev','maxledgerno','on_level','builds_up','level_difficulty']:
    parts.append(function('src/dungeon.c',name))
parts.append(function('src/detect.c','level_distance'))
sf=source('src/sfstruct.c')
parts.append(sf[sf.index('#define MAXFD'):sf.index('#ifdef SFLOGGING',sf.index('#define MAXFD'))])
for name in ['getidx','bwrite','mread','sfstruct_read_error']:
    parts.append(function('src/sfstruct.c',name))
parts.append(sf[sf.index('#define SFO_BODY'):sf.index('#define SFO_CBODY')])
parts.append('SF_A(xint16)')
version=source('src/version.c')
parts.append(version[version.index('struct critical_sizes_with_names {'):
                     version.index('uchar cscbuf[')])
parts.append(function('src/version.c','get_critical_size_count'))
generated=out/'depth_functions.c'
generated.write_text('\n\n'.join(parts),encoding='utf-8')
subprocess.run(['cl','/nologo','/std:c11','/DWIN32','/DWIN32CON',
                '/D_CRT_SECURE_NO_WARNINGS','/I'+str(repo/'include'),
                '/I'+str(repo/'submodules/lua'),str(repo/'test/test_depth_range.c'),
                str(generated),'/Fe:'+str(out/'depth.exe')],cwd=out,check=True)
subprocess.run([str(out/'depth.exe')],cwd=out,check=True)
