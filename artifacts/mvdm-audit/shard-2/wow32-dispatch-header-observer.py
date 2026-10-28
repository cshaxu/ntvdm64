"""Read-only original WOW32 dispatch header roles and selected link evidence."""
import csv, hashlib, json
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
ROOT = 'wow32/'
NAMES = ('witbl.h','wkbtbl.h','wthtbl.h','wowtbl.h')
B = Path('build/.ninja/msvc-x86-release/wow32')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    rows = []
    for name in NAMES:
        rel = ROOT + name; f = frozen[rel]
        hashes = [sha(root / rel) for root in (M, *O)]
        assert hashes == [f['current_sha256'], f['opennt_sha256'], f['opennt45_sha256']]
        rows.append(dict(path=rel, hashes=hashes, dos_catalog_rows=f['dos_catalog_rows'], wow_catalog_rows=f['wow_catalog_rows']))
    names = ('wow32/precomp.h','wow32/wow32.c','wow32/wowtbl.c','wow16/kernel31/kdata.asm')
    ctx = [M / n for n in names]
    pre, entry, table, guest = [p.read_text(encoding='latin1').lower() for p in ctx]
    assert '#include "wowtbl.h"' in pre and '#include "wkbtbl.h"' in entry and '#include "wthtbl.h"' in entry
    assert 'w32 aw32wow[]' in table and '#include "wkbdtbl2.h"' in table and '#include "wthtbl2.h"' in table
    assert 'initthunktableoffsets' in entry and 'w32gettableoffsets' in entry and 'wowtablemodstart' in guest
    graph = (B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    edges = [next(line for line in graph.splitlines() if line.startswith('build obj/'+n+'.obj:')) for n in ('wow32','wowtbl')]
    map_path = B/'wow32.dll.map'; mp = map_path.read_text(encoding='latin1')
    symbols = [next(line.strip() for line in mp.splitlines() if token in line) for token in ('_W32GetTableOffsets@4','_InitThunkTableOffsets@0','_aw32WOW')]
    return dict(result='pass', sources=rows, context=[dict(path=str(p), sha256=sha(p)) for p in ctx],
                graph_path=str(B/'build.ninja'), graph_sha256=sha(B/'build.ninja'), compile_edges=edges,
                map_path=str(map_path), map_sha256=sha(map_path), map_symbols=symbols,
                scope='Header roles, selected object/map evidence and source dispatch references, not guest ABI or per-API runtime proof')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
