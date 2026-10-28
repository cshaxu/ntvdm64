"""Observe WOW small-heap owners and selected call targets in released DLL."""
import csv
import hashlib
import json
import re
import runpy
import struct
import subprocess
from pathlib import Path

A=Path('artifacts/mvdm-audit/shard-3')
B=Path('build/.ninja/msvc-x86-release/wow32')
P=Path('build/t473-s10/shard-3-wowheap-r82')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h=runpy.run_path('artifacts/mvdm-audit/host-tail-image-observer.py')
    root=A.parent
    frozen={r['relative_path']:r for r in csv.DictReader((root/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    graph=(B/'build.ninja').read_text().replace('$:',':')
    link=next(s for s in graph.splitlines() if s.startswith('build wow32.dll |'))
    assert 'obj/wheap.obj' in link.split()
    deps=subprocess.check_output(['ninja','-C',str(B),'-t','deps','obj/wheap.obj'],text=True)
    assert '(VALID)' in deps.splitlines()[0] and 'wheap.h' in deps.lower()
    sources=[]
    inputs=[Path('assets/release/WOW32.DLL'),B/'wow32.dll',B/'wow32.dll.map',B/'build.ninja',B/'obj/wheap.obj']
    for name in ('wheap.c','wheap.h'):
        relative='wow32/'+name
        record=frozen[relative]
        source=Path('src/mvdm')/relative
        original=Path('O:/repos.external/OpenNT/base/mvdm')/relative
        alternate=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/relative
        assert sha(source)==sha(original)==sha(alternate)==record['current_sha256']==record['opennt_sha256']==record['opennt45_sha256']
        assert record['dos_catalog_rows']=='1' and record['wow_catalog_rows']=='2'
        sources.append(dict(relative_path=relative,sha256=sha(source),original_path=str(original),wow_source_ids=record['wow_source_ids']))
        inputs.extend([source,original,alternate])
    image=h['Image'](inputs[0]); cached=h['Image'](inputs[1])
    assert image.base==cached.base and image.code==cached.code
    assert all(image.data[s:e]==cached.data[s:e] for s,e,_ in image.code)
    symbols=h['symbols']((B/'wow32.dll.map').read_text())
    obj=h['Coff'](B/'obj/wheap.obj')
    mapped=[s for s in symbols if s['function'] and s['owner'].lower()=='wheap.obj']
    assert len(mapped)==3
    functions=[h['bind'](image,obj,s) for s in mapped]
    calls=[]
    for stem,caller_name,target_name in (('wow32','_W32DllInitialize@12','_CreateSmallHeap@0'),
        ('waccel','_SetupAccelAlias@16','_malloc_w_small@4'),
        ('waccel','_DestroyAccelAlias@4','_free_w_small@4'),
        ('wcuricon','_AllocCursorIconAlias@0','_malloc_w_small@4'),
        ('wreldc','_StoreDC@16','_malloc_w_small@4')):
        caller=next(s for s in symbols if s['name']==caller_name and s['owner'].lower()==stem+'.obj')
        target=next(s for s in mapped if s['name']==target_name)
        caller_obj=h['Coff'](B/('obj/'+stem+'.obj'))
        body,relocs=caller_obj.function(caller_name)
        positions=[p for p,n,k in relocs if n==target_name and k==20]
        assert len(positions)==1
        pos=positions[0]; assert body[pos-1] in (0xe8,0xe9)
        displacement=struct.unpack('<i',h['data_at'](image,caller['va']+pos,4))[0]
        assert caller['va']+pos+4+displacement==target['va']+struct.unpack_from('<i',body,pos)[0]
        calls.append(dict(caller=caller_name,target=target_name,caller_owner=caller['owner']))
        inputs.append(B/('obj/'+stem+'.obj'))
    return dict(result='pass',sources=sources,functions=functions,calls=calls,
        inputs=[dict(path=str(p),sha256=sha(p)) for p in dict.fromkeys(inputs)],
        scope='Selected heap implementations and five call edges, not concurrency, allocation failure or task cleanup proof')


if __name__=='__main__':
    print(json.dumps(inspect(),indent=2))
