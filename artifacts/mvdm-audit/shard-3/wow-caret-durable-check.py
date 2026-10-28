"""Recheck released caret bodies and dispatch without old temporary package."""
import hashlib
import json
import runpy
import struct
from pathlib import Path

A=Path('artifacts/mvdm-audit/shard-3')
B=Path('build/.ninja/msvc-x86-release/wow32')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def main():
    observed=json.loads((A/'wow-caret-image-observations.json').read_text(encoding='utf-8'))
    assert observed['result']=='pass' and len(observed['functions'])==len(observed['user_slots'])==8
    assert len(observed['native_calls'])==5
    for item in observed['inputs']:
        path=Path(item['path'])
        if 'build/package-uuid/package-c/' in path.as_posix().lower():
            continue
        assert sha(path)==item['sha256'], path
    h=runpy.run_path('artifacts/mvdm-audit/host-tail-image-observer.py')
    image=h['Image'](Path('assets/release/WOW32.DLL'))
    symbols=h['symbols']((B/'wow32.dll.map').read_text())
    obj=h['Coff'](B/'obj/wucaret.obj')
    for record in observed['functions']:
        symbol=next(s for s in symbols if s['name']==record['name'] and s['owner']==record['owner'])
        assert h['bind'](image,obj,symbol)==record
    table=next(s for s in symbols if s['name']=='_aw32WOW' and s['owner'].lower()=='wowtbl.obj')
    for slot in observed['user_slots']:
        value=struct.unpack('<I',h['data_at'](image,table['va']+4*slot['table_index'],4))[0]
        target=next(s for s in symbols if s['name']==slot['symbol'] and s['owner'].lower()=='wucaret.obj')
        assert value==slot['va']==target['va'] and slot['table_index']==311+slot['user_ordinal']
    for call in observed['native_calls']:
        caller=next(s for s in symbols if s['name']==call['caller'] and s['owner'].lower()=='wucaret.obj')
        target=next(s for s in symbols if s['name']==call['target'] and s['owner']==call['target_owner'])
        body,relocs=obj.function(call['caller'])
        offsets=[p for p,n,k in relocs if n==call['target'] and k==20]
        assert len(offsets)==1
        offset=offsets[0]
        disp=struct.unpack('<i',h['data_at'](image,caller['va']+offset,4))[0]
        assert caller['va']+offset+4+disp==target['va']+struct.unpack_from('<i',body,offset)[0]
    print('PASS released caret functions, eight dispatch slots and five native edges; prior guest execution not replayed')


if __name__=='__main__':
    main()
