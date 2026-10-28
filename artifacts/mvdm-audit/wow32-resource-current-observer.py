"""Observe selected WOW32 resource source and bounded released-image strings."""
import hashlib
import json
from pathlib import Path

paths = [Path('src/mvdm/wow32/wow32.rc'),
         Path('O:/repos.external/OpenNT/base/mvdm/wow32/wow32.rc'),
         Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/wow32/wow32.rc')]
hashes = [hashlib.sha256(p.read_bytes()).hexdigest().upper() for p in paths]
assert len(set(hashes)) == 1
release = Path('assets/release/wow32.dll')
data = release.read_bytes()
strings = ('Application Error', 'Local Port', '32-bit WOW Subsystem Library')
present = {s: s.encode('utf-16le') in data for s in strings}
assert all(present.values())
graph = Path('build/.ninja/msvc-x86-release/wow32/build.ninja').read_text(encoding='utf-8')
edges = [line for line in graph.splitlines() if 'wow32.rc' in line or 'obj/wow32.res' in line]
assert any('wow32.rc' in line and 'obj/wow32.res' in line for line in edges)
assert any('WOW32.DLL' in line.upper() and 'obj/wow32.res' in line for line in edges)
print(json.dumps({'source_sha256': hashes[0], 'release_sha256': hashlib.sha256(data).hexdigest().upper(),
                  'strings': present, 'graph_edges': edges,
                  'scope': 'Representative raw strings and source/build edges; not resource ID, locale or runtime proof'}, indent=2))
