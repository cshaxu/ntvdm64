"""Observe OEMUNI private macro header identities and selected consumers."""
import hashlib
import json
import pathlib
import subprocess

sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest().upper()
rel = 'oemuni/oem.h'
roots = ('src/mvdm', 'O:/repos.external/OpenNT/base/mvdm',
         'O:/repos.external/OpenNT-4.5/nt/private/mvdm')
headers = [pathlib.Path(r) / rel for r in roots]
sources = [pathlib.Path('src/mvdm/oemuni') / x for x in ('file.c', 'process.c')]
build = pathlib.Path('build/.ninja/msvc-x86-release/worker')
deps = subprocess.run(['ninja', '-C', str(build), '-t', 'deps'], capture_output=True, check=True).stdout
consumer = []
target = None
for line in deps.decode('utf-8', errors='replace').splitlines():
    if line and not line[0].isspace():
        target = line
    elif target and line.strip().replace('\\', '/').lower().endswith('/src/mvdm/' + rel):
        consumer.append(target)
mp = build / 'ntvdm.exe.map'
names = ('_CreateFileOem@28', '_SearchPathOem@24', '_GetEnvironmentVariableOem@12')
lines = [line.strip() for line in mp.read_text(encoding='utf-8', errors='replace').splitlines()
         if any(n in line for n in names)]
doc = pathlib.Path('docs/etc/evidence/m0-t420-s37-oemuni-capability-review.md')
print(json.dumps({'header_sha256': [sha(p) for p in headers],
                  'source_sha256': {str(p).replace('\\', '/'): sha(p) for p in sources},
                  'deps_sha256': hashlib.sha256(deps).hexdigest().upper(), 'consumers': consumer,
                  'map_sha256': sha(mp), 'map_lines': lines,
                  'historical_doc_sha256': sha(doc),
                  'limits': 'Current dependency/map and historical evidence only; not a current guest or complete OEM error/lifecycle test.'}, indent=2))
