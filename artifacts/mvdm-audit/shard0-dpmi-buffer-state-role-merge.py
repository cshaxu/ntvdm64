"""Classify selected DPMI buffer and saved-state owners as partial."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit'); BASE = '3e779bb74'; PREFIX = 'shard0-dpmi-buffer-state-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/dpmi-buffer-state-observations.json'
observed = json.loads(table.read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'shard-0/dpmi-buffer-state-observer.py'))['inspect']() == observed
assert {x['path'] for x in observed['sources']} == {'dpmi32/buffer.c', 'dpmi32/savestat.c'}
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for item in observed['sources']:
    path = item['path']; row = ledger[path]
    assert row['status'] == 'pending' and row['shard'] == '0'
    proofs.append(dict(relative_path=path, shard='0', sha256=item['sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(),
        original_edge='Original DPMI translation owner compiled and bound to released NTVDM',
        limits='Guest spans, nested state, allocation failures, exception unwind and teardown remain unverified.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: source and image identity do not establish guest behavior', followup_stage='S11-S12'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-0/dpmi-buffer-state-review.md',
    'Original DPMI buffer and saved-state owners remain partial; guest and lifecycle behavior remain in scope.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='Original source and object/image identity; no guest runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
