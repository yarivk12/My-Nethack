# My-NetHack compliance validator created 2026-10-15.
# Original authors retain copyright; distributed under LICENSE and dat/license.
"""Verify exact notice-only source, optionally run a historical source checker.

python test/verify_public_source_compliance.py [test/test_step10b3_3_source.py]
Historical reads are projected only after validating every actual tracked file.
Native builds and runtime checks never receive projected source.
"""
import ast
import hashlib
import json
from pathlib import Path
import re
import runpy
import subprocess
import sys
from unittest.mock import patch
import xml.etree.ElementTree as ET

ROOT=Path(__file__).resolve().parents[1]
BASE='7fa6dd11a544470a4c1bda3cf9c0628bbb73da29'
lf=lambda b:b.replace(b'\r\n',b'\n')
digest=lambda b:hashlib.sha256(lf(b)).hexdigest()
git=lambda *a:subprocess.check_output(['git',*a],cwd=ROOT)


def verify():
    record=json.loads((ROOT/'doc/public-source-modifications.json').read_text())
    assert record['snapshot']==BASE
    entries={}; links={}
    for line in git('ls-tree','-r',BASE).decode().splitlines():
        meta,name=line.split('\t',1)
        if meta.startswith('160000'):links[name]=meta.split()[2]
        else:entries[name]=meta.split()[2]
    assert links==record['dependency_pins']
    assert set(entries)==set(record['files'])|set(record['unchanged_sha256_lf'])
    assert len(entries)==record['regular_files_audited']
    result=subprocess.run(['git','cat-file','--batch'],cwd=ROOT,input=''.join(oid+'\n' for oid in entries.values()).encode(),stdout=subprocess.PIPE,check=True).stdout
    blobs={}; cursor=0
    for name,oid in entries.items():
        end=result.index(b'\n',cursor); actual_oid,kind,length=result[cursor:end].decode().split()
        assert actual_oid==oid and kind=='blob'
        cursor=end+1;length=int(length);blobs[name]=result[cursor:cursor+length];cursor+=length+1
    assert cursor==len(result)
    projected={}; python=xml=0
    for name,oid in entries.items():
        original=blobs[name]
        actual=(ROOT/name).read_bytes()
        before=lf(original)
        row=record['files'].get(name)
        if row is None:
            assert lf(actual)==before and digest(actual)==record['unchanged_sha256_lf'][name],name
            continue
        assert digest(before)==row['before_sha256_lf'],name
        expected=before
        for insert in reversed(row['insertions']):
            pos=insert['offset']; value=insert['text'].encode()
            style=row['format']
            if style=='c-block':assert re.fullmatch(rb'/\*(?:(?!\*/).)*\*/\n',value,re.S),name
            elif style=='xml-comment-after-declaration':assert re.fullmatch(rb'<!--(?:(?!--).)*-->\n',value,re.S),name
            elif style in ('hash-line','lua-line'):
                marker=b'#' if style=='hash-line' else b'--'
                assert all(not line or line.startswith(marker) for line in value.splitlines()),name
            else:assert style in ('plain-prose','markdown-prose'),name
            expected=expected[:pos]+value+expected[pos:]
        assert lf(actual)==expected and digest(actual)==row['after_sha256_lf'],name
        assert digest(actual+b'UNREVIEWED')!=row['after_sha256_lf'],name
        if name.endswith('.py'):
            assert ast.dump(ast.parse(actual))==ast.dump(ast.parse(original)),name
            python+=1
        elif name.endswith(('.props','.vcxproj')):
            assert ET.tostring(ET.fromstring(actual))==ET.tostring(ET.fromstring(original)),name
            xml+=1
        projected[(ROOT/name).resolve()]=original
    for name,oid in links.items():assert git('ls-files','-s','--',name).decode().split()[1]==oid
    assert lf((ROOT/'LICENSE').read_bytes())==lf((ROOT/'dat/license').read_bytes())
    print(f"PASS exact original-body identity: {len(entries)} regular files; {len(links)} dependency pins; {len(record['files'])} notice corrections",flush=True)
    print(f"PASS comment syntax and mutation rejection; {python} Python ASTs and {xml} XML trees unchanged",flush=True)
    return projected


if __name__=='__main__':
    canonical=verify()
    if len(sys.argv)>1:
        target=Path(sys.argv[1]).resolve(); args=sys.argv[2:]
        read_bytes=Path.read_bytes;read_text=Path.read_text
        def projected_bytes(path):return canonical.get(path.resolve(),None) if path.resolve() in canonical else read_bytes(path)
        def projected_text(path,encoding=None,errors=None):
            if path.resolve() in canonical:return canonical[path.resolve()].decode(encoding or 'utf8',errors or 'strict').replace('\r\n','\n')
            return read_text(path,encoding=encoding,errors=errors)
        sys.path.insert(0,str(ROOT/'test'));sys.argv=[str(target)]+args
        with patch.object(Path,'read_bytes',projected_bytes),patch.object(Path,'read_text',projected_text):
            runpy.run_path(str(target),run_name='__main__')
