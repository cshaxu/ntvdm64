"""Return insufficiently supported whole-file decisions to pending, preserving evidence."""
import collections
import csv
import json
import runpy
import subprocess
import sys
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '1977ca649'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
scoped = A / 'nine-way/part-8/existing-classifications-sample101-300-corrections.csv'
with scoped.open(encoding='utf-8-sig', newline='') as stream:
    evidence = list(csv.DictReader(stream))
assert len(evidence) == 200
current = {r['relative_path']: r['status'] for r in H['original_rows'](A / 'combined-initial-status.csv', BASE)}
source_suffixes = {'.asm','.inc','.c','.h','.ext','.ctl','.cl1','.cl2','.cl3','.cl4','.cla','.clb','.clc','.cld','.clf','.skl'}
selected = {r['path'] for r in evidence if current[r['path']] == 'excluded' and Path(r['path']).suffix.lower() in source_suffixes}
assert len(selected) == 81
selected.update(('dos/v86/cmd/fastopen/FASTOPEN.EXE', 'dos/v86/cmd/graphics/GRAPHICS.COM'))
assert len(selected) == 83
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
assert counts == {'pending': 1987, 'excluded': 1881, 'partial': 763, 'full': 53}
summary_path = A / 'initial-summary.json'
summary = json.loads(summary_path.read_text())
summary['counts'] = counts
summary['note'] = 'Overall file decisions reopened where evidence only established a scoped host role or image identity; original evidence is retained.'
summary_path.write_text(json.dumps(summary, indent=2) + '\n')
result = {'result': 'pass', 'base': BASE, 'reopened': len(selected), 'counts': counts,
          'scope': 'Conservative correction of insufficient overall dispositions; no source, runtime or guest change',
          'evidence_sha256': {str(scoped): H['sha'](scoped)}}
(A / 'classification-scope-correction-101-300-verification.json').write_text(json.dumps(result, indent=2) + '\n')
print(json.dumps(result))
