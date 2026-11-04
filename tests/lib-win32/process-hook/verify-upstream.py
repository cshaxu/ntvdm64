from hashlib import sha256
from pathlib import Path
import sys

root = Path(__file__).resolve().parents[3] / 'src/lib-win32/process-hook'
manifest = Path(__file__).with_name('upstream-sha256.txt')
overrides = {}
for line in Path(__file__).with_name('downstream-sha256.txt').read_text(encoding='ascii').splitlines():
    digest, name = line.split('  ', 1)
    if name in overrides:
        raise SystemExit(f'duplicate downstream override: {name}')
    overrides[name] = digest
seen = set()
for line in manifest.read_text(encoding='ascii').splitlines():
    digest, name = line.split('  ', 1)
    path = Path(name)
    if path.is_absolute() or '..' in path.parts or name in seen:
        raise SystemExit(f'invalid manifest path: {name}')
    seen.add(name)
    actual = sha256((root / path).read_bytes()).hexdigest()
    expected = overrides.get(name, digest)
    if actual != expected:
        raise SystemExit(f'undocumented source difference: {name}')
if len(seen) != 23:
    raise SystemExit(f'expected 23 pinned upstream files, found {len(seen)}')
if set(overrides) != {'helper.c'} or not set(overrides).issubset(seen):
    raise SystemExit('unexpected downstream override set')
print(f'PASS {len(seen)-len(overrides)} byte-exact upstream files and {len(overrides)} documented local fix')
