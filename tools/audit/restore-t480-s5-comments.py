"""Apply six reviewed original same-line comments, preserving every other byte."""
import csv
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORK = ROOT / 'build/t480-s5'


def main():
    plan = json.loads((WORK / 'restore-plan.json').read_text())
    pins = {r['path']: r for r in csv.DictReader((ROOT / 'artifacts/mvdm-audit/t480-s1-files.csv').open(encoding='utf-8-sig'))}
    for arch in ('x86', 'x64'):
        result = json.loads((WORK / f'pp-before-{arch}.result.json').read_text(encoding='utf-8-sig'))
        assert result['exitCode'] == 0
    pending = []
    for row in plan['changes']:
        path = ROOT / row['path']
        raw = path.read_bytes()
        assert hashlib.sha256(raw).hexdigest() == row['before_sha256']
        original = Path(pins[row['path']]['original']).read_bytes()
        assert hashlib.sha256(original).hexdigest() == row['original_sha256']
        assert row['after'].encode('latin1') in original.splitlines()
        lines = raw.splitlines(keepends=True)
        old = lines[row['line'] - 1]
        assert old.rstrip(b'\r\n') == row['before'].encode('latin1')
        lines[row['line'] - 1] = row['after'].encode('latin1') + old[len(old.rstrip(b'\r\n')):]
        new = b''.join(lines)
        assert len(new.splitlines()) == len(raw.splitlines())
        row['after_sha256'] = hashlib.sha256(new).hexdigest()
        pending.append((path, new))
    for path, data in pending:
        path.write_bytes(data)
    (WORK / 'restorations.json').write_text(json.dumps(plan, indent=2)+'\n', encoding='utf-8')
    print('Restored six pinned same-line comments; no other bytes changed')


if __name__ == '__main__':
    main()
