"""Per-file lexical host include and recorded dependency-name census.

Run from the repository root; writes JSON to stdout only. A lexical include
is not proof of preprocessing or resolution, and matching names are not
evidence that different paths denote the same header.
"""
import collections
import csv
import hashlib
import io
import json
import pathlib
import re
import subprocess

root = pathlib.Path('src/mvdm')
inventory = pathlib.Path('artifacts/mvdm-audit/host-ch-scope/inventory.csv')
raw = inventory.read_bytes()
rows = list(csv.DictReader(io.StringIO(raw.decode('utf-8-sig'), newline='')))
guest = [x for x in rows if x['scope'] in ('role-pending', 'guest-target-observed')]
host = [x for x in rows if x['scope'] == 'host-candidate']
by_name = collections.defaultdict(list)
for r in guest:
    by_name[pathlib.PurePosixPath(r['path']).name.casefold()].append(r['path'])
includes = collections.defaultdict(list)
for r in host:
    path = root / r['path']
    if not path.is_file():
        continue
    data = path.read_bytes()
    sha = hashlib.sha256(data).hexdigest().upper()
    for n, line in enumerate(data.decode('utf-8', errors='replace').splitlines(), 1):
        match = re.match(r'\s*#\s*include\s*[<"]([^>"]+)[>"]', line)
        if match:
            name = pathlib.PurePosixPath(match.group(1).replace('\\', '/')).name.casefold()
            if name in by_name:
                includes[name].append({'host_path': r['path'], 'line': n,
                                       'spelling': match.group(1), 'host_sha256': sha})
resolved = collections.defaultdict(set)
trees = ('msvc-x86-release/worker', 'msvc-x86-release/wow32',
         'msvc-x64-release/worker', 'pkg-uuid-x86/worker')
graphs = []
for tree in trees:
    build = pathlib.Path('build/.ninja') / tree
    graph = (build / 'build.ninja').read_bytes()
    run = subprocess.run(['ninja', '-C', str(build), '-t', 'deps'], capture_output=True, check=True)
    graphs.append({'tree': tree, 'graph_sha256': hashlib.sha256(graph).hexdigest().upper(),
                   'deps_sha256': hashlib.sha256(run.stdout).hexdigest().upper()})
    for line in run.stdout.decode('utf-8', errors='replace').splitlines():
        if line and line[0].isspace():
            path = line.strip().replace('\\', '/')
            name = pathlib.PurePosixPath(path).name.casefold()
            if name in by_name:
                resolved[name].add((tree, path))
out = []
for r in guest:
    file = root / r['path']
    name = pathlib.PurePosixPath(r['path']).name.casefold()
    out.append({'path': r['path'], 'ledger_status': r['ledger_status'],
                'sha256': hashlib.sha256(file.read_bytes()).hexdigest().upper() if file.is_file() else None,
                'same_basename_host_includes': includes[name],
                'same_basename_recorded_dependencies': [dict(tree=t, path=p) for t, p in sorted(resolved[name])]})
print(json.dumps({'inventory_sha256': hashlib.sha256(raw).hexdigest().upper(),
                  'graphs': graphs, 'rows': out,
                  'limits': 'Lexical names and recorded dependency paths only; neither proves original selected branches or guest-image behavior.'}, indent=2))
