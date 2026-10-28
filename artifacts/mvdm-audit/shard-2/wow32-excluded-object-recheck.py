"""Read-only recheck of three behaviorally excluded but selected WOW32 objects."""
import csv, hashlib, json, re, struct
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/wow32')
NAMES = ('wgman','wmmedia1','wthman')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper()

def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    ledger = {r['relative_path']: r for r in csv.DictReader((A/'combined-initial-status.csv').open(encoding='utf-8-sig'))}
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    link=next(line for line in graph.splitlines() if line.startswith('build wow32.dll | wow32.lib:'))
    map_path=B/'wow32.dll.map'; mp=map_path.read_text(encoding='latin1')
    rows=[]
    for name in NAMES:
        rel='wow32/'+name+'.c'; f=frozen[rel]; paths=[root/rel for root in (M,*O)]
        hashes=[sha(p) for p in paths]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        assert hashes[0] == hashes[1] == hashes[2]
        assert ledger[rel]['status'] == 'excluded'
        edge=next(line for line in graph.splitlines() if line.startswith('build obj/'+name+'.obj:'))
        assert 'obj/'+name+'.obj' in link.split()
        obj=B/('obj/'+name+'.obj'); raw=obj.read_bytes()
        machine,count,_,_,_,optional,_=struct.unpack_from('<HHIIIHH',raw,0)
        assert machine == 0x14c and optional == 0
        sections=[]
        for i in range(count):
            off=20+i*40; flags=struct.unpack_from('<I',raw,off+36)[0]
            sections.append(dict(name=raw[off:off+8].rstrip(b'\0').decode('ascii'), flags=flags,
                                 size=struct.unpack_from('<I',raw,off+16)[0]))
            assert not flags & 0x20
        owners=[line.strip() for line in mp.splitlines() if re.search(r'(?i)\b'+name+r'\.obj\s*$',line)]
        assert not owners
        source=paths[0].read_text(encoding='latin1')
        if name in ('wgman','wthman'):
            stripped=re.sub(r'/\*.*?\*/','',source,flags=re.S)
            lines=[line.strip() for line in stripped.splitlines() if line.strip()]
            assert lines == ['#include "precomp.h"','#pragma hdrstop','MODNAME('+name+'.c);']
        else:
            assert re.search(r'#pragma\s+hdrstop\s*\n\s*#if\s+0\b',source)
            assert re.search(r'#endif\s*\Z',source)
        rows.append(dict(path=rel,status=ledger[rel]['status'],source_sha256=hashes[0],
                         compile_edge=edge,link_input=True,object_sha256=sha(obj),sections=sections,map_owner_lines=owners))
    return dict(result='pass',rows=rows,graph_sha256=sha(B/'build.ninja'),map_sha256=sha(map_path),
                scope='Captured release objects are link inputs with no code sections; not absence from build or proof of related guest functionality')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
