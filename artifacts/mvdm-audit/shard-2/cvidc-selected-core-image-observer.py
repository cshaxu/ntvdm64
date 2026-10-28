"""Read-only selected C-VID core source/object/released-code sample bindings."""
import csv,hashlib,json,runpy
from pathlib import Path

A,M=Path('artifacts/mvdm-audit'),Path('src/mvdm')
O=(Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B=Path('build/.ninja/msvc-x86-release/worker')
ITEMS=(('softpc.new/base/cvidc/accessfn.c','accessfn','_PurgeLostIretHookLine'),
       ('softpc.new/base/cpu/src/evid/vglob.c','vglob','_getVideobit_prot_mask'),
       ('softpc.new/base/cvidc/sinit013.c','SINIT013','_S_0415_CiSetVideofwd_str_read_addr_00000034'))

def sha(p): return hashlib.sha256(Path(p).read_bytes()).hexdigest().upper()

def inspect():
    h=runpy.run_path(str(A/'host-tail-image-observer.py'))
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    ledger={r['relative_path']:r for r in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))}
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    archive=next(line for line in graph.splitlines() if line.startswith('build original-softpc-cvidc.lib:'))
    release=Path('assets/release/ntvdm.exe'); image=h['Image'](release); cached=h['Image'](B/'ntvdm.exe')
    assert image.base == cached.base and image.code == cached.code
    assert all(image.data[s:e] == cached.data[s:e] for s,e,_ in image.code)
    symbols=h['symbols']((B/'ntvdm.exe.map').read_text())
    rows=[]
    for rel,objname,func in ITEMS:
        f=frozen[rel]; hashes=[sha(root/rel) if (root/rel).is_file() else '' for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        assert ledger[rel]['shard']=='2' and ledger[rel]['status']=='pending'
        target='obj/cvidc/'+objname+'.obj'
        edge=next(line for line in graph.splitlines() if line.lower().startswith(('build '+target+':').lower()))
        assert target.lower() in archive.lower().split()
        symbol=next(s for s in symbols if s['name']==func and s['owner'].lower().endswith(':'+objname.lower()+'.obj') and s['function'])
        obj=B/target; binding=h['bind'](image,h['Coff'](obj),symbol)
        rows.append(dict(path=rel,hashes=hashes,compile_edge=edge,object_sha256=sha(obj),sample_binding=binding))
    return dict(result='pass',sources=rows,release_sha256=sha(release),cached_sha256=sha(B/'ntvdm.exe'),
                map_sha256=sha(B/'ntvdm.exe.map'),graph_sha256=sha(B/'build.ninja'),
                scope='Three relocation-masked sample function bindings to released worker; no complete runtime, vector or device lifecycle proof')

if __name__ == '__main__': print(json.dumps(inspect(),indent=2))
