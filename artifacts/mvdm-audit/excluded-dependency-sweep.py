"""Inventory current excluded roles that appear in selected Ninja graphs/deps."""
import csv
import hashlib
import json
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
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
    ledger = A / 'combined-initial-status.csv'
    rows = [r for r in csv.DictReader(ledger.open(encoding='utf-8-sig')) if r['status'] == 'excluded']
    hits = []
    for row in rows:
        path = row['relative_path']; suffix = '/src/mvdm/' + path.lower()
        found = [dict(root=root, graph=suffix in graph, deps=suffix in deps) for root,graph,deps,_ in graphs if suffix in graph or suffix in deps]
        if found: hits.append(dict(path=path, shard=row['shard'], matches=found))
    return dict(result='pass', ledger_sha256=sha(ledger), excluded_count=len(rows), candidate_hits=hits,
                graphs=[dict(root=root, graph_sha256=digest) for root,_,_,digest in graphs],
                limits='Normalized path substring candidates in four cached graphs/dependency outputs, not proof of object extraction, preprocessor activity, all build profiles or feature status.')
if __name__ == '__main__': print(json.dumps(inspect(), indent=2))
