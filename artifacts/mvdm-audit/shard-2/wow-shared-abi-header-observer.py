"""Read-only shared WOW guest/host interface and normalized dependency evidence."""
import csv, hashlib, json, subprocess
from pathlib import Path

A, M = Path('artifacts/mvdm-audit'), Path('src/mvdm')
O = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
B = Path('build/.ninja/msvc-x86-release/wow32').resolve()
NAMES = ('WOWCMDLG.INC','wowcomm.h','wowgdi.h','WOWGDI.INC','wowkbd.h')

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''

def inspect():
    frozen={r['relative_path']:r for r in csv.DictReader((A/'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    deps=subprocess.run(['ninja','-C',str(B),'-t','deps','obj/wowtbl.obj'],check=True,capture_output=True,text=True).stdout
    paths={str((B/line.strip()).resolve()).lower() for line in deps.splitlines() if line.startswith('    ')}
    rows=[]
    for name in NAMES:
        rel='inc/'+name; f=frozen[rel]; hashes=[sha(root/rel) for root in (M,*O)]
        assert hashes == [f['current_sha256'],f['opennt_sha256'],f['opennt45_sha256']]
        selected=str((M/rel).resolve()).lower() in paths
        if name.endswith('.h'): assert selected
        rows.append(dict(path=rel,hashes=hashes,dos_catalog_rows=f['dos_catalog_rows'],
                         wow_catalog_rows=f['wow_catalog_rows'],selected_normalized_dependency=selected))
    names=('inc/makefile','wow16/gdi/gdi.asm','wow16/gdi/makefile','wow16/commdlg/commdlg.asm',
           'wow16/commdlg/makefile','wow16/drivers/keyboard/keyboard.asm','wow32/precomp.h','wow32/wucomm.h')
    ctx=[M/n for n in names]
    mk,gdi,gdib,dlg,dlgb,kbd,pre,comm=[p.read_text(encoding='latin1').lower() for p in ctx]
    assert 'wowgdi.inc:   wowgdi.h wow.inc' in mk and 'wowcmdlg.inc: wowcmdlg.h wow.inc' in mk
    assert 'include wowgdi.inc' in gdi and 'wowgdi.inc' in gdib
    assert 'include wowcmdlg.inc' in dlg and 'wowcmdlg.inc' in dlgb
    assert 'include wowkbd.inc' in kbd and '#include "wowkbd.h"' in pre
    assert '#include "wowgdi.h"' in pre and '#include "wowcomm.h"' in comm
    graph=(B/'build.ninja').read_text(encoding='utf-8').replace('$:',':')
    edge=next(line for line in graph.splitlines() if line.startswith('build obj/wowtbl.obj:'))
    return dict(result='pass',sources=rows,context=[dict(path=str(p),sha256=sha(p)) for p in ctx],
                normalized_deps_object='obj/wowtbl.obj',compile_edge=edge,graph_sha256=sha(B/'build.ninja'),
                scope='Header dependency and historical guest include roles; no exact ABI, generated .inc provenance or runtime proof')

if __name__ == '__main__':
    print(json.dumps(inspect(),indent=2))
