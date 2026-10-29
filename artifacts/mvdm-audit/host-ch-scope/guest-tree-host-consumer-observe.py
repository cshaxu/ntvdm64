"""Read-only snapshot of exact guest-tree C/H references in four Ninja graphs.

Run from repository root. Output JSON to stdout; does not update any ledger.
Recorded dependency groups are not proof that all host profiles were built.
"""
import csv
import hashlib
import json
import pathlib
import subprocess

inventory = pathlib.Path('artifacts/mvdm-audit/host-ch-scope/inventory.csv')
raw = inventory.read_bytes()
import io
rows = [r for r in csv.DictReader(io.StringIO(raw.decode('utf-8-sig'), newline=''))
        if r['scope'] in ('role-pending', 'guest-target-observed')]
paths = {r['path'].replace('\\', '/').lower() for r in rows}
trees = ('msvc-x86-release/worker', 'msvc-x86-release/wow32',
         'msvc-x64-release/worker', 'pkg-uuid-x86/worker')
out = []
for name in trees:
    build = pathlib.Path('build/.ninja') / name
    graph = (build / 'build.ninja').read_bytes()
    graph_text = graph.decode('utf-8', errors='replace').lower().replace('\\', '/').replace('$:', ':')
    run = subprocess.run(['ninja', '-C', str(build), '-t', 'deps'], capture_output=True, check=True)
    deps = run.stdout.decode('utf-8', errors='replace').splitlines()
    deps_paths = set()
    for line in deps:
        line = line.strip().replace('\\', '/').lower()
        marker = '/src/mvdm/'
        at = line.find(marker)
        if at >= 0:
            deps_paths.add(line[at + len(marker):])
    out.append({
        'tree': name,
        'graph_sha256': hashlib.sha256(graph).hexdigest().upper(),
        'dependency_output_sha256': hashlib.sha256(run.stdout).hexdigest().upper(),
        'dependency_groups': sum('#deps ' in line for line in deps),
        'valid_groups': sum('#deps ' in line and '(VALID)' in line for line in deps),
        'candidate_graph_references': sorted(p for p in paths if '/src/mvdm/' + p in graph_text),
        'candidate_dependency_paths': sorted(paths & deps_paths),
    })
print(json.dumps({'inventory_sha256': hashlib.sha256(raw).hexdigest().upper(),
                  'candidate_rows': len(rows), 'trees': out,
                  'limits': 'Recorded groups and named graphs only; absence is not a per-file exclusion.'},
                 indent=2))
