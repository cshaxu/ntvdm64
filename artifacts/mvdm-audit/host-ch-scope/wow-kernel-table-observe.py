"""Read-only current identity, dependency and historical table observations."""
import hashlib
import json
import pathlib
import subprocess

sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest().upper()
rel = 'wow32/wktbl2.h'
roots = ('src/mvdm', 'O:/repos.external/OpenNT/base/mvdm',
         'O:/repos.external/OpenNT-4.5/nt/private/mvdm')
paths = [pathlib.Path(r) / rel for r in roots]
source = pathlib.Path('src/mvdm/wow32/wowtbl.c')
build = pathlib.Path('build/.ninja/msvc-x86-release/wow32')
deps = subprocess.run(['ninja', '-C', str(build), '-t', 'deps'], capture_output=True, check=True).stdout
current = None
matches = []
for line in deps.decode('utf-8', errors='replace').splitlines():
    if line and not line[0].isspace():
        current = line
    elif current and line.strip().replace('\\', '/').lower().endswith('/src/mvdm/' + rel):
        matches.append(current)
mapfile = build / 'wow32.dll.map'
lines = [line.strip() for line in mapfile.read_text(encoding='utf-8', errors='replace').splitlines()
         if '_aw32WOW ' in line or '_WK32GetDriveType@4' in line]
receipt = pathlib.Path('artifacts/mvdm-audit/shard-1/wow-table-boundary-observations.json')
old = json.loads(receipt.read_text(encoding='utf-8'))
old_header = next(x for x in old['inputs'] if x['path'].replace('\\', '/').lower().endswith('/wow32/wktbl2.h'))
print(json.dumps({'header_sha256': [sha(p) for p in paths], 'wowtbl_source_sha256': sha(source),
                  'wowtbl_includes_header': '#include "wktbl2.h"' in source.read_text(),
                  'deps_sha256': hashlib.sha256(deps).hexdigest().upper(), 'dependency_groups': matches,
                  'map_sha256': sha(mapfile), 'map_lines': lines,
                  'historical_receipt_sha256': sha(receipt), 'historical_result': old['result'],
                  'historical_kernel_part': old['parts'][0],
                  'historical_header_matches_current': old_header['sha256'].upper() == sha(paths[0]),
                  'limits': 'Source, deps and cached-image observations only; table entries include local/unimplemented sentinels and do not establish live API behavior.'}, indent=2))
