"""Link two existing exception registrations to their partial-source rows."""
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
ledger = A / 'shard-3/file-review.csv'
rows = H['rows'](ledger)
registry = Path('src/mvdm/README.md').read_text(encoding='utf-8')
updates = {
    'inc/mvdm.h': ('MVDM-SUPPORT-DIV-006',),
    'softpc.new/base/ccpu386/cpu4gen.h': ('MVDM-HOST-DIV-046', 'MVDM-HOST-DIV-048'),
}
seen = set()
for row in rows:
    path = row['relative_path']
    if path not in updates:
        continue
    seen.add(path)
    assert row['review_class'] == 'partial' and not row['local_divergence_ids']
    for token in updates[path]:
        assert any(token in line and path in line for line in registry.splitlines()), (path, token)
    row['local_divergence_ids'] = ';'.join(updates[path])
    if path.endswith('cpu4gen.h'):
        row['specific_limits'] = 'DIV-046 and DIV-048 are registered; selected ABI, branches and runtime behavior remain to be verified.'
        row['specific_evidence'] += '; src/mvdm/README.md'
assert seen == set(updates)
H['write_csv_like_base'](ledger, rows, '3452abace')
print('linked two existing registrations; classifications unchanged')
