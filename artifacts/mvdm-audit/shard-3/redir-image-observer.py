"""Bind three original redirector owners and dispatch to released DLL."""
import csv
import hashlib
import json
import runpy
import struct
import subprocess
from pathlib import Path

A=Path('artifacts/mvdm-audit')
B=Path('build/.ninja/msvc-x86-release/worker')
MAP=A/'shard-3/redir-private-map.txt'
ROOT=Path('build/t473-s10/shard-3-redir-map-r81')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    h=runpy.run_path(str(A/'host-tail-image-observer.py'))
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    graph=(B/'build.ninja').read_text().replace('$:',':')
    link=next(s for s in graph.splitlines() if s.startswith('build VDMREDIR.dll |'))
    deps=subprocess.check_output(['ninja','-C',str(B),'-t','deps'],text=True)
    release=Path('assets/release/VDMREDIR.DLL')
    image=h['Image'](release)
    relink=h['Image'](ROOT/'VDMREDIR.dll')
    assert image.base==relink.base and image.code==relink.code
    assert all(image.data[s:e]==relink.data[s:e] for s,e,_ in image.code)
    assert MAP.read_bytes()==(ROOT/'VDMREDIR.dll.map').read_bytes()
    symbols=h['symbols'](MAP.read_text())
    inputs=[release,ROOT/'VDMREDIR.dll',MAP,B/'build.ninja']
    sources=[]
    for stem,count in (('vrdll',1),('vrnetb',5),('vrdlcbuf',8)):
        relative='vdmredir/'+stem+'.c'; record=frozen[relative]
        source=Path('src/mvdm')/relative
        one=Path('O:/repos.external/OpenNT/base/mvdm')/relative
        two=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')/relative
        assert sha(source)==sha(one)==sha(two)==record['current_sha256']==record['opennt_sha256']==record['opennt45_sha256']
        assert record['dos_catalog_rows']=='1' and record['wow_catalog_rows']=='2'
        target='obj/redir/'+stem+'.obj'
        assert target in link.split()
        edge=next(s for s in graph.splitlines() if s.startswith('build '+target+':'))
        assert source.resolve().as_posix().lower() in edge.lower()
        dep=next(s for s in deps.splitlines() if s.startswith(target+':'))
        assert '(VALID)' in dep and '(STALE)' not in dep
        obj=h['Coff'](B/target)
        mapped=[s for s in symbols if s['function'] and s['owner'].lower()==stem+'.obj']
        assert len(mapped)==count
        sources.append(dict(relative_path=relative,sha256=sha(source),original_path=str(one),
            wow_source_ids=record['wow_source_ids'],compile_edge=edge,dependency_header=dep,
            functions=[h['bind'](image,obj,s) for s in mapped]))
        inputs.extend([source,one,two,B/target])
    # Check the initial redirector dispatch table's INT 5Ch service slot.
    obj=h['Coff'](B/'obj/redir/vrdisp.obj')
    definition=[s for s in obj.symbols.values() if s[0]=='_VrDispatchTable' and s[2]>0]
    assert len(definition)==1
    _,start,section=definition[0]; body,relocs=obj.sections[section-1]; pos=start+0x25*4
    assert [(n,k) for p,n,k in relocs if p==pos]==[('_VrNetbios5c',6)]
    table=next(s for s in symbols if s['name']=='_VrDispatchTable' and s['owner'].lower()=='vrdisp.obj')
    selected=next(s for s in symbols if s['name']=='_VrNetbios5c' and s['owner'].lower()=='vrnetb.obj')
    value=struct.unpack('<I',h['data_at'](image,table['va']+0x25*4,4))[0]
    assert value==(selected['va']+struct.unpack_from('<I',body,pos)[0])&0xffffffff
    calls=[]
    requests=(('vdmredir-dll/mvdm_redirector_dll_entry','_mvdm_vdmredir_dll_entry@12','_VrDllInitialize'),
              ('redir/vrdll','_VrDllInitialize','_VrInitialize'),
              ('redir/vrdll','_VrDllInitialize','_VrUninitialize'),
              ('redir/vrdll','_VrDllInitialize','_TerminateDlcEmulation'),
              ('redir/vrdlc5c','_ProcessImmediateCommand','_FreeBuffers'),
              ('redir/vrdlc5c','_ProcessImmediateCommand','_GetBuffers'),
              ('redir/vrdlc5c','_ProcessImmediateCommand','_CreateBufferPool'),
              ('redir/vrdlc5c','_ProcessImmediateCommand','_DeleteBufferPool'))
    for path,caller_name,target_name in requests:
        caller=next(s for s in symbols if s['name']==caller_name and s['function'])
        target_symbol=next(s for s in symbols if s['name']==target_name and s['function'])
        obj=h['Coff'](B/('obj/'+path+'.obj'))
        body,relocs=obj.function(caller_name)
        positions=[p for p,n,k in relocs if n==target_name and k==20]
        assert len(positions)==1
        offset=positions[0]
        assert body[offset-1] in (0xe8,0xe9) or (offset>=2 and body[offset-2]==0x0f and 0x80<=body[offset-1]<=0x8f)
        displacement=struct.unpack('<i',h['data_at'](image,caller['va']+offset,4))[0]
        assert caller['va']+offset+4+displacement==target_symbol['va']+struct.unpack_from('<i',body,offset)[0]
        calls.append(dict(caller=caller_name,target=target_name,caller_owner=caller['owner'],target_owner=target_symbol['owner']))
        inputs.append(B/('obj/'+path+'.obj'))
    inputs.append(B/'obj/redir/vrdisp.obj')
    return dict(result='pass',sources=sources,netbios_dispatch=dict(index=0x25,va=value,target=selected['name']),
        calls=calls,inputs=[dict(path=str(p),sha256=sha(p)) for p in dict.fromkeys(inputs)],
        scope='Initial image dispatch and selected call targets, not DLL lifecycle, NetBIOS callbacks or DLC behavior')


if __name__=='__main__':
    print(json.dumps(inspect(),indent=2))
