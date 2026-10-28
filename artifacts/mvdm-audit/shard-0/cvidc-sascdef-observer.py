"""Record selected CCPU40 SAS vector source identity and map attribution."""
import hashlib
import json
from pathlib import Path

root = Path('src/mvdm/softpc.new/base')
paths = [root / 'cvidc/sascdef.c', root / 'ccpu386/sascdef.c']
manifest_path = Path('build/.ninja/msvc-x86-release/worker/source-manifest.json')
map_path = Path('build/.ninja/msvc-x86-release/worker/ntvdm.exe.map')
manifest = json.loads(manifest_path.read_text(encoding='utf-8-sig'))
lines = [line.strip() for line in map_path.read_text(errors='replace').splitlines()
         if '_cSasPtrs' in line]
assert manifest['cpuProfile'] == 'CCPU40'
assert manifest['originalCcpuManifest'].lower().endswith('ccpu386/sources')
assert any(name.lower() == 'sascdef.c' for name in manifest['ccpuSources'])
assert len(lines) == 1 and 'original-ccpu386:sasCdef.obj'.lower() in lines[0].lower()
hashes = {str(path): hashlib.sha256(path.read_bytes()).hexdigest() for path in paths}
assert len(set(hashes.values())) == 2
print(json.dumps({'source_sha256': hashes,
                  'manifest_sha256': hashlib.sha256(manifest_path.read_bytes()).hexdigest(),
                  'map_sha256': hashlib.sha256(map_path.read_bytes()).hexdigest(),
                  'selected_profile': manifest['cpuProfile'], 'symbol_lines': lines}, indent=2))
