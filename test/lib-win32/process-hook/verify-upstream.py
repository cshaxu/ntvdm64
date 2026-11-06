from hashlib import sha256
from pathlib import Path
root = Path(__file__).resolve().parents[3] / 'src/lib-win32/process-hook'
seen = set()
for line in Path(__file__).with_name('upstream-sha256.txt').read_text(encoding='ascii').splitlines():
    digest, name = line.split('  ', 1)
    path = Path(name)
    if path.is_absolute() or '..' in path.parts or name in seen:
        raise SystemExit(f'invalid manifest path: {name}')
    seen.add(name)
    if sha256((root / path).read_bytes()).hexdigest() != digest:
        raise SystemExit(f'upstream source difference: {name}')
actual = {p.relative_to(root).as_posix() for p in root.rglob('*') if p.is_file()}
if actual != seen:
    raise SystemExit(f'source inventory mismatch: {actual ^ seen}')
print(f'PASS {len(seen)} byte-exact upstream files; no downstream source overrides')
