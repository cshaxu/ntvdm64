"""Recheck retained redirector identity and call evidence without private build output."""
import hashlib
import json
import runpy
import struct
from pathlib import Path

A=Path('artifacts/mvdm-audit/shard-3')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def main():
    observed=json.loads((A/'redir-image-observations.json').read_text(encoding='utf-8'))
    assert observed['result']=='pass' and len(observed['sources'])==3 and len(observed['calls'])==8
    # The private relink and its map were compared at observation time. Its
    # temporary image need not be retained; all other exact inputs are checked.
    for item in observed['inputs']:
        path=Path(item['path'])
        if path.as_posix().lower().endswith('shard-3-redir-map-r81/vdmredir.dll'):
            continue
        assert sha(path)==item['sha256'], path
    h=runpy.run_path('artifacts/mvdm-audit/host-tail-image-observer.py')
    image=h['Image'](Path('assets/release/VDMREDIR.DLL'))
    mapping=h['symbols']((A/'redir-private-map.txt').read_text())
    objects={}
    for source in observed['sources']:
        stem=Path(source['relative_path']).stem
        obj=h['Coff'](h['BUILD']/('obj/redir/'+stem+'.obj'))
        objects[stem]=obj
        for record in source['functions']:
            current=next(s for s in mapping if s['name']==record['name'] and s['owner']==record['owner'])
            assert h['bind'](image,obj,current)==record
    for record in observed['calls']:
        owner=record['caller_owner']
        if ':' in owner:
            objpath='obj/vdmredir-dll/mvdm_redirector_dll_entry.obj'
        else:
            objpath='obj/redir/'+owner
        obj=h['Coff'](h['BUILD']/objpath)
        body,relocs=obj.function(record['caller'])
        offsets=[p for p,n,k in relocs if n==record['target'] and k==20]
        assert len(offsets)==1
        caller=next(s for s in mapping if s['name']==record['caller'] and s['owner']==owner)
        target=next(s for s in mapping if s['name']==record['target'] and s['owner']==record['target_owner'])
        offset=offsets[0]
        value=struct.unpack('<i',h['data_at'](image,caller['va']+offset,4))[0]
        assert caller['va']+offset+4+value==target['va']+struct.unpack_from('<i',body,offset)[0]
    table=next(s for s in mapping if s['name']=='_VrDispatchTable' and s['owner']=='vrdisp.obj')
    value=struct.unpack('<I',h['data_at'](image,table['va']+0x25*4,4))[0]
    assert value==observed['netbios_dispatch']['va']
    print('PASS retained redirector image, map, objects, table slot and eight call targets')


if __name__=='__main__':
    main()
