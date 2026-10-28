"""Read-only WOW32 USER header, source and selected-map role observations."""
import csv, hashlib, json
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/wow32')
ROOT = 'wow32/'
NAMES = ('wuclip.h','wucomm.h','wudlg.h','wuman.h','wuser31.h')
ENTRIES = (('wuclip','WU32OpenClipboard'),('wucomm','WU32OpenComm'),
           ('wudlg','WU32DialogBoxParam'),('wuman','WU32MouseEvent'),('wuser31','WU32SetWindowPlacement'))

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
    pre = (M/'wow32/precomp.h').read_text(encoding='latin1').lower()
    table = (M/'wow32/wutbl2.h').read_text(encoding='latin1')
    graph = (B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    map_path = B/'wow32.dll.map'; mp = map_path.read_text(encoding='latin1')
    entries = []
    for owner, name in ENTRIES:
        assert '#include "'+owner+'.h"' in pre
        src = M/('wow32/'+owner+'.c')
        assert name in src.read_text(encoding='latin1') and name in table
        edge = next(line for line in graph.splitlines() if line.startswith('build obj/'+owner+'.obj:'))
        symbol = next(line.strip() for line in mp.splitlines() if '_'+name+'@4' in line and owner+'.obj' in line)
        entries.append(dict(header=owner+'.h', source=str(src), source_sha256=sha(src), function=name, compile_edge=edge, map_entry=symbol))
    assert 'WINDOWPLACEMENT16TO32' in (M/'wow32/wuser31.h').read_text(encoding='latin1')
    assert 'WINDOWPLACEMENT16TO32' in (M/'wow32/wuser31.c').read_text(encoding='latin1')
    return dict(result='pass', sources=rows, sample_entries=entries,
                precomp_sha256=sha(M/'wow32/precomp.h'), table_sha256=sha(M/'wow32/wutbl2.h'),
                graph_sha256=sha(B/'build.ninja'), map_sha256=sha(map_path),
                scope='Header inclusion and sample source/table/map selection; no per-entry execution or lifecycle proof')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
