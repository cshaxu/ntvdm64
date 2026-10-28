"""Classify original inactive converter source bodies without excluding multimedia."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'bee94b5e2', 'wow-multimedia-disabled-structures-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/wow-multimedia-disabled-structures-observations.json'
observed = runpy.run_path(str(A / 'shard-0/wow-multimedia-disabled-structures-observer.py'))['inspect']()
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for source in observed['files']:
    path = source['relative_path']
    assert len(set(source['hashes'])) == 1 and ledger[path]['shard'] == '0'
    assert ledger[path]['status'] in ('pending', 'excluded')
    proofs.append(dict(relative_path=path, shard='0', sha256=source['hashes'][0], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='excluded',
        role='original-disabled-source-body-only', evidence_table=table.relative_to(A).as_posix(),
        original_edge='Original converter body is unconditionally disabled; linked current object has no loadable code or data',
        limits='Multimedia guest, provider, conversion, callbacks and task lifecycle remain in scope.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: original disabled file body, not multimedia capability', followup_stage='S14-S16'))
assert len(proofs) == 2
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-0/wow-multimedia-disabled-structures-review.md',
    'Two original disabled converter file roles excluded; multimedia capability remains open.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='Original disabled source bodies only, not multimedia capability')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
