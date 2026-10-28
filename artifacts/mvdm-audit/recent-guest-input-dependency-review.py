"""Recheck recent guest-input exclusions against normalized Ninja paths."""
import csv
import hashlib
import json
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
GROUPS = ('parallel-gdi-ctl3d', 'parallel-ddeml-wfwnet', 'parallel-debug-write', 'parallel-shld-tools', 'dosx-guest')
ROOTS = ('build/.ninja/msvc-x86-release/worker', 'build/.ninja/msvc-x86-release/wow32',
         'build/.ninja/msvc-x64-release/frontend', 'build/.ninja/msvc-x64-release/service')
def sha(path): return hashlib.sha256(Path(path).read_bytes()).hexdigest().upper()
def inspect():
    graphs = []
    for root in ROOTS:
        path = Path(root) / 'build.ninja'
        graph = path.read_text().replace('$:', ':').replace('\\', '/').lower()
        deps = subprocess.check_output(['ninja', '-C', root, '-t', 'deps'], text=True, encoding='utf-8', errors='replace').replace('\\', '/').lower()
        graphs.append((root, graph, deps, sha(path)))
    entries = []
    for group in GROUPS:
        path = A / (group + '-role-dispositions.csv')
        for row in csv.DictReader(path.open(encoding='utf-8-sig')):
            if row['status'] != 'excluded': continue
            rel = row['relative_path']; suffix = '/src/mvdm/' + rel.lower()
            hits = [root for root, graph, deps, _ in graphs if suffix in graph or suffix in deps]
            assert not hits, (rel, hits)
            entries.append(dict(path=rel, checkpoint=group, checked_graph_hits=hits))
    assert len(entries) == 37 and len({x['path'] for x in entries}) == 37
    return dict(result='pass', entries=entries, graphs=[dict(root=root, graph_sha256=digest) for root,_,_,digest in graphs],
                limitations='Exact normalized paths in four current graphs/dependency logs only; no guest functionality, source lineage or other build-profile proof.')
if __name__ == '__main__': print(json.dumps(inspect(), indent=2))
