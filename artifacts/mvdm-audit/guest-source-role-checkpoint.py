"""Verify the bounded S10 guest-source-role checkpoint, not guest behavior."""
import argparse
import collections
import csv
import hashlib
import io
import json
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = 'd050fa4b4'


def rows(path):
    with path.open(encoding='utf-8-sig', newline='') as stream:
        return list(csv.DictReader(stream))


def original_rows(path, base):
    data = subprocess.check_output(['git', 'show', f'{base}:{path.as_posix()}'])
    return list(csv.DictReader(io.StringIO(data.decode('utf-8-sig'))))


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def main(output, base=BASE, prefix='guest-source-role-', expected_count=187):
    frozen_path = A / 'frozen-file-reconciliation.csv'
    assert sha(frozen_path) == 'F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196'
    frozen = {r['relative_path']: r for r in rows(frozen_path)}
    merged_path = A / 'combined-initial-status.csv'
    merged = {r['relative_path']: r for r in rows(merged_path)}
    old = {r['relative_path']: r for r in original_rows(merged_path, base)}
    dispositions = rows(A / (prefix + 'dispositions.csv'))
    changed = {r['relative_path']: r for r in dispositions}
    assert len(dispositions) == len(changed) == expected_count
    assert len(frozen) == len(merged) == len(old) == 4684
    assert set(frozen) == set(merged) == set(old)
    delta = {p for p in merged if merged[p] != old[p]}
    assert delta == set(changed)
    for path, row in merged.items():
        assert int.from_bytes(hashlib.sha256(path.encode()).digest()[:8], 'big') % 4 == int(row['shard'])
        if path in changed:
            expected = dict(old[path], status='excluded')
            assert old[path]['status'] == changed[path]['prior_status'] == 'pending'
            assert row == expected
            proof = changed[path]
            assert proof['role'] == 'historical-guest-build-source-file-only'
            assert sha(Path('src/mvdm') / path) == sha(Path(proof['original_path'])) == proof['sha256']
            assert proof['sha256'] == frozen[path]['current_sha256']
            assert proof['sha256'] in (frozen[path]['opennt_sha256'], frozen[path]['opennt45_sha256'])
            assert proof['original_edge'] and proof['limits']
    counts = dict(collections.Counter(r['status'] for r in merged.values()))
    expected_counts = collections.Counter(r['status'] for r in old.values())
    expected_counts['pending'] -= expected_count
    expected_counts['excluded'] += expected_count
    assert counts == expected_counts
    summary = json.loads((A / 'initial-summary.json').read_text())
    assert counts == summary['counts'] and summary['complete'] is False
    specs = [('shard-0/review.csv', 'classification'), ('shard-1/file-audit.csv', 'audit_status'),
             ('shard-2/file-audit.csv', 'decision'), ('shard-3/file-review.csv', 'review_class')]
    translations = {'待核验': 'pending', '部分复通': 'partial', '整体排除': 'excluded',
                    '全复通': 'full', '全部复通': 'full'}
    covered = set()
    for shard, (name, column) in enumerate(specs):
        source_path = A / name
        previous = {r['relative_path']: r for r in original_rows(source_path, base)}
        for row in rows(source_path):
            path = row['relative_path']
            assert path not in covered and int(merged[path]['shard']) == shard
            covered.add(path)
            assert translations.get(row[column], row[column]) == merged[path]['status']
            if path not in changed:
                assert row == previous[path], path
    assert covered == set(merged)
    observation = json.loads((A / (prefix + 'observations.json')).read_text())
    assert observation['result'] == 'pass' and observation['candidate_count'] == expected_count
    for record in observation['source_tables']:
        assert sha(A / record['path']) == record['sha256']
    for record in observation['host_input_observations']:
        path = Path(record['path'])
        assert sha(path) == record['sha256'] and '(STALE)' not in path.read_text()
        text = path.read_text().replace('\\', '/').lower()
        for relative in changed:
            assert 'src/mvdm/' + relative.lower() not in text, relative
    for row in rows(A / 'full-selected-leaf.csv') + rows(A / 'full-selected-declarations.csv'):
        assert merged[row['relative_path']]['status'] == 'full'
        assert sha(Path('src/mvdm') / row['relative_path']) == row['source_sha256']
    drift = []
    for path, row in frozen.items():
        source = Path('src/mvdm') / path
        if not source.exists():
            assert not row['current_sha256']
            continue
        current = sha(source)
        if current != row['current_sha256']:
            drift.append(dict(relative_path=path, frozen=row['current_sha256'], current=current))
    assert drift == summary['live_source_hash_drift']
    result = dict(result='pass', base=base, counts=counts, source_role_changes=expected_count,
                  unrelated_shard_rows_unchanged=True, frozen_inventory_unchanged=True,
                  product_or_guest_changed_by_checkpoint=False,
                  scope='Source roles, identities, recorded input absence and ledger consistency; no guest capability acceptance')
    output.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output', type=Path)
    parser.add_argument('--base', default=BASE)
    parser.add_argument('--prefix', choices=('guest-source-role-', 'kernel31-source-role-'), default='guest-source-role-')
    parser.add_argument('--expected-count', type=int, default=187)
    args = parser.parse_args()
    main(args.output, args.base, args.prefix, args.expected_count)
