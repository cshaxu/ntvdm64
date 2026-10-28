"""Read-only multimedia and message declarations, normalized Ninja dependency evidence."""
import csv, hashlib, json, subprocess
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/wow32').resolve()
NAMES = ('wmmedia.h','wmmtbl2.h','wmtbl32.h')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    rows = []
    deps = subprocess.run(['ninja','-C',str(B),'-t','deps','obj/wowtbl.obj'], check=True, capture_output=True, text=True).stdout
    paths = {str((B / line.strip()).resolve()).lower() for line in deps.splitlines() if line.startswith('    ')}
    for name in NAMES:
        rel = 'wow32/'+name; f = frozen[rel]
        hashes = [sha(root / rel) for root in (M, *O)]
        assert hashes == [f['current_sha256'], f['opennt_sha256'], f['opennt45_sha256']]
        assert str((M/rel).resolve()).lower() in paths
        rows.append(dict(path=rel, hashes=hashes, dos_catalog_rows=f['dos_catalog_rows'],
                         wow_catalog_rows=f['wow_catalog_rows'], normalized_dependency=str((M/rel).resolve())))
    pre=(M/'wow32/precomp.h').read_text(encoding='latin1').lower()
    tbl=(M/'wow32/wowtbl.c').read_text(encoding='latin1').lower()
    mm=(M/'wow32/wmmtbl2.h').read_text(encoding='latin1').lower()
    ext=Path('src/wow32-dll/source/wow_user_native_extension.c').read_text(encoding='latin1').lower()
    assert '#include "wmmedia.h"' in pre and '#include "wmtbl32.h"' in pre and '#include "wmmtbl2.h"' in tbl
    assert 'wmm32callproc32' in mm and 'wow_original_wmm32callproc32' in ext
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    edges=[next(line for line in graph.splitlines() if line.startswith('build obj/'+name+'.obj:')) for name in ('wowtbl','wmmedia','wmtbl32')]
    map_path=B/'wow32.dll.map'; mp=map_path.read_text(encoding='latin1')
    symbols=[next(line.strip() for line in mp.splitlines() if token in line.split()) for token in ('_wow_original_WMM32CallProc32@4','_WMM32CallProc32@4','_aw32Msg')]
    return dict(result='pass', sources=rows, normalized_deps_object='obj/wowtbl.obj', compile_edges=edges,
                graph_sha256=sha(B/'build.ninja'), map_sha256=sha(map_path), map_symbols=symbols,
                extension_sha256=sha(Path('src/wow32-dll/source/wow_user_native_extension.c')),
                scope='Normalized dependency, source and map snapshot; no linked table relocation, live API or callback lifetime proof')

if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
