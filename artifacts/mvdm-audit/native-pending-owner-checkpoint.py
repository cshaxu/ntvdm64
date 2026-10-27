"""Check pending-owner inventory identities, not whole-file behavior."""
import csv
import hashlib
import json
import sys
from pathlib import Path

A = Path('artifacts/mvdm-audit')


def rows(path):
    with path.open(encoding='utf-8-sig', newline='') as stream:
        return list(csv.DictReader(stream))


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def main(output):
    frozen = {r['relative_path']: r for r in rows(A / 'frozen-file-reconciliation.csv')}
    ledger = {r['relative_path']: r for r in rows(A / 'combined-initial-status.csv')}
    specs = [(1, 164, 'actual_current_sha256', 'disposition'),
             (2, 137, 'current_sha256', 'candidate'),
             (3, 153, 'actual_sha256', 'candidate')]
    seen = set()
    tables = []
    for shard, expected, hash_column, status_column in specs:
        path = A / f'shard-{shard}/native-pending-owner-review.csv'
        data = rows(path)
        intended = {p for p, r in ledger.items() if r['status'] == 'pending'
                    and int(r['shard']) == shard
                    and p.startswith(('softpc.new/base/', 'softpc.new/host/'))
                    and Path(p).suffix.lower() in ('.c', '.h', '.asm', '.inc')}
        assert len(data) == expected and {r['relative_path'] for r in data} == intended
        for row in data:
            rel = row['relative_path']
            assert rel not in seen and row[status_column] == 'pending'
            seen.add(rel)
            assert int.from_bytes(hashlib.sha256(rel.encode()).digest()[:8], 'big') % 4 == shard
            assert sha(Path('src/mvdm') / rel) == row[hash_column] == frozen[rel]['current_sha256']
            assert row['frozen_sha256'] == frozen[rel]['current_sha256']
            for root, column in [('O:/repos.external/OpenNT/base/mvdm', 'opennt_sha256'),
                                 ('O:/repos.external/OpenNT-4.5/nt/private/mvdm', 'opennt45_sha256')]:
                if frozen[rel][column]:
                    assert sha(Path(root) / rel) == frozen[rel][column]
                for recorded_column in (column, 'frozen_' + column, 'actual_' + column):
                    if recorded_column in row:
                        assert row[recorded_column] == frozen[rel][column]
        tables.append(dict(path=path.as_posix(), sha256=sha(path), rows=len(data)))
    assert len(seen) == 454
    result = dict(result='pass', pending_rows=454, source_tables=tables,
                  main_native_shard_review_complete=False, ledger_changes=0,
                  scope='Identity, exact cohort, partition and pending status only; not call-graph or behavior acceptance')
    output.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result))


if __name__ == '__main__':
    main(Path(sys.argv[1]))
