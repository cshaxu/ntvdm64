"""Record original debugger provider identity, selected graph and historical witness."""
import hashlib
import json
import pathlib

sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest().upper()
rel = 'dbg/dbg.c'
roots = ('src/mvdm', 'O:/repos.external/OpenNT/base/mvdm',
         'O:/repos.external/OpenNT-4.5/nt/private/mvdm')
src = [pathlib.Path(r) / rel for r in roots]
build = pathlib.Path('build/.ninja/msvc-x86-release/worker')
graph, obj, mapfile = (build / 'build.ninja', build / 'obj/mvdm-debugger/dbg.obj',
                       build / 'ntvdm.exe.map')
evidence = pathlib.Path('docs/etc/evidence/m0-t420-s43-debugger-capability-ledger.md')
source = pathlib.Path('src/mvdm/dos/dem/demmisc.c')
graphlines = graph.read_text(encoding='utf-8', errors='replace').splitlines()
maplines = mapfile.read_text(encoding='utf-8', errors='replace').splitlines()
needles = ('_DBGDispatch ', '_GetNormalContext ', '_ModuleLoad ', '_SendVDMEvent ')
text = evidence.read_text(encoding='utf-8', errors='replace')
print(json.dumps({'source_identity': [sha(p) for p in src],
                  'selected_object_sha256': sha(obj), 'graph_sha256': sha(graph),
                  'graph_edges': [x for x in graphlines if 'obj/mvdm-debugger/dbg.obj' in x],
                  'map_sha256': sha(mapfile),
                  'map_lines': [x.strip() for x in maplines if any(n in x for n in needles)],
                  'demmisc_sha256': sha(source), 'historical_ledger_sha256': sha(evidence),
                  'historical_marker_present': 'S43_DEBUGGER_REAL_WORKER_OEM_MODULE_OK' in text,
                  'limits': 'Source/graph/map and historical ledger only; no current debugger execution or current install identity proof.'}, indent=2))
