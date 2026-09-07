"""VS developer shell: python test/run_step8a.py OUTPUT_DIRECTORY.

Compile production function bodies and real databases, then execute contracts.
"""
from pathlib import Path
import re
import subprocess
import sys

repo = Path(__file__).resolve().parents[1]
out = Path(sys.argv[1]).resolve()
subprocess.run([sys.executable, str(repo/'test/run_step7.py'), str(out)],check=True)
parts=[]
for path,names in {
    'src/dungeon.c':['moria_level','moria_sky','moria_rndmonst'],
    'src/do.c':['moria_unique_chain'],
    'src/mhitu.c':['fern_release'],
    'src/trap.c':['swamp_effects'],
    'src/exper.c':['experience'],
    'src/mon.c':['swamp_spore_dies'],
}.items():
    text=(repo/path).read_text()
    for name in names:
        matches=list(re.finditer(r'(?m)^(?:staticfn )?(?:struct permonst \*|\w+)\n'+name+r'\([\s\S]*?^\}',text))
        assert len(matches)==1,(path,name)
        parts.append(matches[0][0])
(out/'step8a_functions.h').write_text('\n\n'.join(parts))
subprocess.run(['cl','/nologo','/std:c11','/W4','/D_CRT_SECURE_NO_WARNINGS',
    '/DWIN32','/DWIN32CON','/I'+str(repo/'include'),'/I'+str(repo/'submodules/lua'),
    '/I'+str(out),'/Fe:'+str(out/'step8a_runtime.exe'),
    str(repo/'test/test_step8a_runtime.c'),str(repo/'src/monst.c'),str(repo/'src/objects.c')],cwd=out,check=True)
subprocess.run([str(out/'step8a_runtime.exe')],cwd=out,check=True)

# Compile the exact allocator with the bundled Lua runtime, including __gc.
source=(repo/'src/nhlua.c').read_text()
struct=re.search(r'typedef struct nhl_user_data \{[\s\S]*?\} nhl_user_data;',source)[0]
allocator=re.search(r'staticfn void \*\nnhl_alloc\([\s\S]*?^\}',source,re.M)[0]
(out/'step8a_allocator.h').write_text(struct+'\n'+allocator)
lua_sources=[str(p) for p in (repo/'submodules/lua').glob('*.c') if p.stem not in ('lua','luac','onelua')]
subprocess.run(['cl','/nologo','/std:c11','/D_CRT_SECURE_NO_WARNINGS',
    '/I'+str(repo/'submodules/lua'),'/I'+str(out),'/Fe:'+str(out/'step8a_lua.exe'),
    str(repo/'test/test_step8a_lua.c')]+lua_sources,cwd=out,check=True)
subprocess.run([str(out/'step8a_lua.exe')],cwd=out,check=True)
