"""Read-only per-file role evidence for provisional tool/scaffold paths."""
import csv
import hashlib
import io
import json
import pathlib
import re
import subprocess

inventory = pathlib.Path('artifacts/mvdm-audit/host-ch-scope/inventory.csv')
raw = inventory.read_bytes()
rows = [r for r in csv.DictReader(io.StringIO(raw.decode('utf-8-sig'), newline=''))
        if r['scope'] == 'tool-or-scaffold-candidate']
roots = [pathlib.Path('src/mvdm'), pathlib.Path('O:/repos.external/OpenNT/base/mvdm'),
         pathlib.Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')]
trees = ('msvc-x86-release/worker', 'msvc-x86-release/wow32',
         'msvc-x64-release/worker', 'pkg-uuid-x86/worker')
graphs = []
for tree in trees:
    build = pathlib.Path('build/.ninja') / tree
    graph = (build / 'build.ninja').read_bytes()
    dep = subprocess.run(['ninja', '-C', str(build), '-t', 'deps'], capture_output=True, check=True).stdout
    graphs.append((tree, graph, dep))
out = []
for row in rows:
    path = row['path']
    hashes = [hashlib.sha256((root / path).read_bytes()).hexdigest().upper()
              if (root / path).is_file() else None for root in roots]
    parent = pathlib.PurePosixPath(path).parent
    source_file = roots[1] / parent / 'sources'
    source_bytes = source_file.read_bytes() if source_file.is_file() else b''
    name = pathlib.PurePosixPath(path).name
    output = {'path': path, 'ledger_status': row['ledger_status'], 'sha256_current_opennt_opennt45': hashes,
              'original_local_sources': str(source_file).replace('\\', '/') if source_bytes else None,
              'original_local_sources_sha256': hashlib.sha256(source_bytes).hexdigest().upper() if source_bytes else None,
              'basename_in_local_sources': bool(re.search(r'(?i)(?<![a-z0-9_])' + re.escape(name) + r'(?![a-z0-9_])', source_bytes.decode('latin1'))),
              'graph_refs': [], 'recorded_deps': []}
    needle = '/src/mvdm/' + path.casefold()
    for tree, graph, dep in graphs:
        normalized_graph = graph.decode('utf-8', errors='replace').casefold().replace('\\', '/').replace('$:', ':')
        if needle in normalized_graph:
            output['graph_refs'].append(tree)
        for line in dep.decode('utf-8', errors='replace').splitlines():
            if line and line[0].isspace() and line.strip().replace('\\', '/').casefold().endswith(needle):
                output['recorded_deps'].append({'tree': tree, 'path': line.strip()})
    out.append(output)
print(json.dumps({'inventory_sha256': hashlib.sha256(raw).hexdigest().upper(),
                  'graphs': [{'tree': t, 'graph_sha256': hashlib.sha256(g).hexdigest().upper(),
                              'deps_sha256': hashlib.sha256(d).hexdigest().upper()} for t, g, d in graphs],
                  'rows': out,
                  'limits': 'Manifest basename is lexical evidence only; headers, conditional profiles, alternate builds and runtime behavior require independent review.'}, indent=2))
