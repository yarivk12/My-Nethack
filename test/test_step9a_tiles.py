"""Check generated glyph references against tile text and name reused art.

Run after a Windows Release build; reads generated files without changing them.
"""
from pathlib import Path
import re
import struct
import subprocess

repo=Path(__file__).resolve().parents[1]
base='5ce8b8193e4c581dd293ccac2bd0cafb4da89e96'
generated=(repo/'src/tile.c').read_text(encoding='utf8')

def tiles(source):
    return [(int(i),name,''.join(body.split())) for i,name,body in
            re.findall(r'# tile (\d+) \(([^\n]+)\)\s*'
                       r'(?:#_[^\n]*\s*)?\{([^}]+)\}',source)]

for file in ['monsters.txt','objects.txt','other.txt']:
    path='win/share/'+file
    current=tiles((repo/path).read_text(encoding='utf8'))
    original=tiles(subprocess.check_output(['git','show',base+':'+path],cwd=repo).decode())
    old_names={name for _,name,_ in original}
    assert len({i for i,_,_ in current})==len(current),file
    for index,name,pixels in current:
        assert len(pixels)==256,(file,name,'bad tile dimensions')
        if name in old_names:continue
        # Generated mapping comments identify source indices. Every new
        # entry must be reachable, including male/female glyph variants.
        assert re.search(re.escape(file)+':0*'+str(index)+r'\b',generated),(file,name)
        if file == 'other.txt' and name == 'forge':
            # Step 15A deliberately introduced original forge artwork. Pin
            # that exact reviewed asset; retain donor-reuse checks elsewhere.
            forge_source=subprocess.check_output(['git','show',
                '17d070c1290cafe64c7f40d52a2888521477ecd0:'+path],cwd=repo).decode()
            approved=[p for _,n,p in tiles(forge_source) if n=='forge']
            assert approved==[pixels],(file,name,'changed Step 15A artwork')
            print('PASS glyph/tile',file,index,name,'matches pinned Step 15A artwork')
            continue
        originals=[n for _,n,p in original if p==pixels]
        assert originals,(file,name,'undocumented new artwork')
        print('PASS glyph/tile',file,index,name,'reuses',originals[0])

bitmap=(repo/'win/win32/tiles.bmp').read_bytes()
assert bitmap[:2]==b'BM'
width,height=struct.unpack_from('<ii',bitmap,18)
count=int(re.search(r'int total_tiles_used = (\d+)',generated)[1])
assert width%16==0
# tile2bmp reserves extra capacity with an integer-divided pixel-height
# formula; unused trailing rows need not form a complete tile row.
assert ((count+width//16-1)//(width//16))*16<=abs(height)
print('PASS generated bitmap capacity',width,abs(height),'for',count,'tiles')
