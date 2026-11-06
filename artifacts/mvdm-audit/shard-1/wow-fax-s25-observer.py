"""Selected fax object/image binding; no external fax execution claim."""
import hashlib
import json
from pathlib import Path
import re
import runpy
import struct
import subprocess
import sys

sys.dont_write_bytecode = True
h = runpy.run_path('artifacts/mvdm-audit/host-tail-image-observer.py')
b = Path('build/.ninja/msvc-x86-release/wow32')
src = Path('src/mvdm-host/wow32/wow32fax.c')
package = Path('build/t478-s25/package-final/system32/WOW32.DLL')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper()

def body(s, name):
    match = re.search(r'^(?:VOID|UINT|LPSTR\s*\*|PSZ|BOOL)\s*'+name+r'\s*\(', s, re.M)
    assert match, name
    start = s.index('{', match.start())
    i, depth = start+1, 1
    while depth:
        if s[i] == '{': depth += 1
        elif s[i] == '}': depth -= 1
        i += 1
    text = s[start:i]
    text = re.sub(r'#ifdef\s+DBG\b.*?#endif', '', text, flags=re.S)
    text = re.sub(r'^#line[^\n]*', '', text, flags=re.M)
    text = re.sub(r'/\*.*?\*/|//[^\n]*', '', text, flags=re.S)
    return re.sub(r'\s+', '', text)

baseline = subprocess.check_output(['git','show','1d8bae55c:src/mvdm-host/wow32/wow32fax.c']).decode('utf-8')
originals = [Path(root)/'wow32/wow32fax.c' for root in
             ('O:/repos.external/OpenNT/base/mvdm', 'O:/repos.external/OpenNT-4.5/nt/private/mvdm')]
names = ['SortedInsert','BuildStrList','GetSupportedFaxDrivers','BuildPath','InstallWowFaxPrinter']
for original in originals:
    text = original.read_text(encoding='cp1252')
    for name in names:
        assert body(text,name) == body(baseline,name), (original,name)
for name in ('SortedInsert','BuildStrList','BuildPath'):
    assert body(src.read_text(encoding='utf-8'),name) == body(baseline,name), name
graph = (b/'build.ninja').read_text(encoding='utf-8').replace('$:', ':')
edge = next(x for x in graph.splitlines() if x.startswith('build obj/wow32fax.obj:'))
assert src.resolve().as_posix().lower() in edge.lower()
link = next(x for x in graph.splitlines() if x.startswith('build wow32.dll |'))
assert 'obj/wow32fax.obj' in link.split()
image, cached = h['Image'](package), h['Image'](b/'wow32.dll')
assert image.code == cached.code
assert all(image.data[a:z] == cached.data[a:z] for a,z,_ in image.code)
syms = h['symbols']((b/'wow32.dll.map').read_text(encoding='utf-8'))
obj = h['Coff'](b/'obj/wow32fax.obj')
functions, edges = [], []
for sym in [s for s in syms if s['owner']=='wow32fax.obj' and s['function']]:
    functions.append(h['bind'](image,obj,sym))
    code, relocs = obj.function(sym['name'])
    for offset,target,kind in relocs:
        if not target.startswith('_wow_registry_'): continue
        assert kind==20
        dst=next(s for s in syms if s['name']==target and s['function'])
        word=struct.unpack('<I',h['data_at'](image,sym['va']+offset,4))[0]
        add=struct.unpack_from('<i',code,offset)[0]
        assert word==(dst['va']+add-(sym['va']+offset+4))&0xffffffff
        edges.append(dict(caller=sym['name'],target=target,owner=dst['owner']))
assert any(e['caller']=='_WOW32FaxHandler@8' and e['target']=='_wow_registry_query_value_ex@24' for e in edges)
assert any(e['caller']=='_InstallWowFaxPrinter@12' and e['target']=='_wow_registry_set_value_ex@24' for e in edges)
paths=[src,*originals,b/'build.ninja',b/'obj/wow32fax.obj',b/'wow32.dll.map',package]
fixtures=[]
for label in ('baseline','confirmed-current'):
    root=Path('build/t478-s25')/label
    fixtures.append(dict(label=label,output=(root/'run.txt').read_text(encoding='utf-8'),
        inputs=[dict(path=str(root/name),sha256=sha(root/name))
                for name in ('fax_exact.h','fax-test.exe','run.txt')]))
print(json.dumps(dict(result='pass',original_release_bodies=names,build_edge=edge,
    functions=functions,registry_edges=edges,isolated_fixtures=fixtures,
    full=json.loads(Path('build/t478-s25/verify-final-Full/timings.json').read_text(encoding='utf-8-sig')),
    publication=Path('build/t478-s25/publication.txt').read_text(encoding='utf-8-sig'),
    inputs=[dict(path=str(p),sha256=sha(p)) for p in paths],
    limitations='Static selected-object/image identity plus original provenance only; not external fax, guest fax or spooler acceptance.'),indent=2))
