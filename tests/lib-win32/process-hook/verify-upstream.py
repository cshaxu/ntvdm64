from hashlib import sha256
from pathlib import Path
import sys

root = Path(__file__).resolve().parents[3] / 'src/lib-win32/process-hook'
manifest = Path(__file__).with_name('upstream-sha256.txt')
seen = set()
for line in manifest.read_text(encoding='ascii').splitlines():
    digest, name = line.split('  ', 1)
    path = Path(name)
    if path.is_absolute() or '..' in path.parts or name in seen:
        raise SystemExit(f'invalid manifest path: {name}')
    seen.add(name)
    actual = sha256((root / path).read_bytes()).hexdigest()
    if actual != digest:
        raise SystemExit(f'upstream source differs: {name}')
if len(seen) != 23:
    raise SystemExit(f'expected 23 pinned upstream files, found {len(seen)}')
print(f'PASS {len(seen)} byte-exact imported process-hook files')
