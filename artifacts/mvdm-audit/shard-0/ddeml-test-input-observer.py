"""Trace shard-0 DDEML test inputs without building or running guest code."""
import csv
import hashlib
import json
from pathlib import Path

A = Path('artifacts/mvdm-audit')
M = Path('src/mvdm')
ROOT = 'wow16/ddeml/tests/'
NAMES = ('ddestrs/cmdln.c', 'ddestrs/ddestrs.h', 'ddestrs/globals.c', 'ddestrs/server.c',
         'ddestrs/wrapper.h', 'src/client/dde.c', 'src/client/dialog.h', 'src/client/infoctrl.c',
         'src/client/infoctrl.h', 'src/client/mk4dos.cmd', 'src/ddemo/ddemo.def',
         'src/ddemo/mk4dos.cmd', 'src/server/mk4dos.cmd', 'src/server/server.h')
CONTEXT = ('ddestrs/sources', 'ddestrs/client.c', 'src/client/sources', 'src/client/ddemlcl.h',
           'src/client/dde.c', 'src/client/infoctrl.c', 'src/ddemo/makefile.dos', 'src/server/sources',
           'src/server/dde.c')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()


def inspect():
    frozen = {r['relative_path']: r for r in csv.DictReader((A / 'frozen-file-reconciliation.csv').open(encoding='utf-8-sig'))}
    ledger = {r['relative_path']: r for r in csv.DictReader((A / 'combined-initial-status.csv').open(encoding='utf-8-sig'))}
    originals = (Path('O:/repos.external/OpenNT/base/mvdm'), Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'))
    sources = []
    for name in NAMES:
        path = ROOT + name
        current = M / path
        row = frozen[path]
        assert ledger[path]['shard'] == '0'
        assert sha(current) == sha(originals[0] / path) == sha(originals[1] / path) == row['current_sha256'] == row['opennt_sha256'] == row['opennt45_sha256']
        sources.append(dict(relative_path=path, sha256=sha(current), wow_source_ids=row['wow_source_ids']))
    texts = {}
    for name in CONTEXT:
        path = M / (ROOT + name)
        assert sha(path) == sha(originals[0] / (ROOT + name)) == sha(originals[1] / (ROOT + name))
        texts[name] = path.read_text(encoding='latin1').lower()
    stress = texts['ddestrs/sources']
    assert 'majorcomp=test' in stress and 'targetname=ddestrs' in stress
    assert all(x in stress for x in ('cmdln.c', 'globals.c', 'server.c'))
    assert '#include "wrapper.h"' in texts['ddestrs/client.c'] and '#include "ddestrs.h"' in texts['ddestrs/client.c']
    client = texts['src/client/sources']
    assert 'majorcomp=samples' in client and 'targetname=ddemlcl' in client and 'dde.c' in client and 'infoctrl.c' in client
    assert '#include "dialog.h"' in texts['src/client/ddemlcl.h']
    assert '#include "infoctrl.h"' in texts['src/client/dde.c'] and '#include "infoctrl.h"' in texts['src/client/infoctrl.c']
    assert 'ddemo.def' in texts['src/ddemo/makefile.dos'] and 'ddemo.exe' in texts['src/ddemo/makefile.dos']
    assert 'targetname=ddemlsv' in texts['src/server/sources'] and '#include "server.h"' in texts['src/server/dde.c']
    for path in ('src/client/mk4dos.cmd', 'src/ddemo/mk4dos.cmd', 'src/server/mk4dos.cmd'):
        assert 'nmake -f makefile.dos' in (M / (ROOT + path)).read_text(encoding='latin1').lower()
    graphs = (Path('build/.ninja/msvc-x86-release/worker/build.ninja'), Path('build/.ninja/msvc-x86-release/wow32/build.ninja'))
    for graph in graphs:
        text = graph.read_text(encoding='utf-8').replace('\\', '/').lower()
        assert all((ROOT + name).lower() not in text for name in NAMES)
    return dict(result='pass', sources=sources,
        context=[dict(path=str(M / (ROOT + n)), sha256=sha(M / (ROOT + n))) for n in CONTEXT],
        graph_inputs=[dict(path=str(p), sha256=sha(p)) for p in graphs],
        scope='Original independent test/sample file roles only; no DDE capability exclusion or runtime acceptance')


if __name__ == '__main__':
    print(json.dumps(inspect(), indent=2))
