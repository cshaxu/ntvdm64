"""Return insufficiently supported whole-file decisions to pending, preserving evidence."""
import collections
import csv
import json
import runpy
import subprocess
import sys
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = 'b8aaf0870'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
files = [A / 'nine-way/part-1/remaining-guest-current-identity-evidence.json',
         A / 'nine-way/part-1/remaining-message-extra-evidence.json']
selected = set()
for scoped in files:
    evidence = json.loads(scoped.read_text(encoding='utf-8-sig'))
    name = scoped.name.replace('-evidence.json', '-observer.py')
    assert evidence == json.loads(subprocess.check_output([sys.executable, '-B', str(scoped.with_name(name))]))
    for row in evidence['rows']:
        hashes = row['hashes']
        assert hashes[0] and hashes[1] and hashes[2] and hashes[0] == hashes[1] == hashes[2]
        assert H['sha'](Path('src/mvdm') / row['path']).lower() == hashes[0]
        selected.add(row['path'])
assert len(selected) == 92
scoped = files[0]
merged_path = A / 'combined-initial-status.csv'
merged = H['rows'](merged_path)
old = {r['relative_path']: r for r in H['original_rows'](merged_path, BASE)}
for row in merged:
    path = row['relative_path']
    assert row == old[path]
    if path in selected:
        assert row['status'] in ('partial', 'excluded')
        row['status'] = 'pending'
H['write_csv_like_base'](merged_path, merged, BASE)
specs = [('shard-0/review.csv', 'classification'), ('shard-1/file-audit.csv', 'audit_status'),
         ('shard-2/file-audit.csv', 'decision'), ('shard-3/file-review.csv', 'review_class')]
seen = set()
for name, column in specs:
    path = A / name
    rows = H['rows'](path)
    old_rows = {r['relative_path']: r for r in H['original_rows'](path, BASE)}
    changed = False
    for row in rows:
        key = row['relative_path']
        assert row == old_rows[key]
        if key not in selected:
            continue
        seen.add(key)
        changed = True
        row[column] = '待核验' if column in ('classification', 'audit_status') else 'pending'
        for field in ('remaining_evidence', 'next_required_evidence', 'review_limits', 'remaining_proof'):
            if field in row:
                row[field] += '; Overall disposition reopened: scoped host role or image identity alone does not prove full capability or exclusion.'
    if changed:
        H['write_csv_like_base'](path, rows, BASE)
assert seen == selected
counts = dict(collections.Counter(r['status'] for r in merged))
assert counts == {'pending': 2175, 'excluded': 1707, 'partial': 749, 'full': 53}
summary_path = A / 'initial-summary.json'
summary = json.loads(summary_path.read_text())
summary['counts'] = counts
summary['note'] = 'Overall file decisions reopened where evidence only established a scoped host role or image identity; original evidence is retained.'
summary_path.write_text(json.dumps(summary, indent=2) + '\n')
result = {'result': 'pass', 'base': BASE, 'reopened': len(selected), 'counts': counts,
          'scope': 'Conservative correction of insufficient overall dispositions; no source, runtime or guest change',
          'evidence_sha256': {str(f): H['sha'](f) for f in files}}
(A / 'classification-scope-correction-guest-remaining-verification.json').write_text(json.dumps(result, indent=2) + '\n')
print(json.dumps(result))
