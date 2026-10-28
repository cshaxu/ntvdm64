"""Reconcile four selected original XMS runtime owner roles."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'c5949d0f6', 'xms-runtime-owner-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/xms-runtime-owner-observations.json'
assert runpy.run_path(str(A / 'shard-0/xms-runtime-owner-observer.py'))['inspect']() == json.loads(table.read_text(encoding='utf-8-sig'))
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs = []
for item in json.loads(table.read_text(encoding='utf-8-sig'))['sources']:
    path, hashes = item['path'], item['hashes']
    row, source = ledger[path], frozen[path]
    assert row['status'] in ('pending', 'partial') and row['shard'] == '0'
    assert hashes[0] == hashes[1] == hashes[2] == source['current_sha256']
    assert item['bindings']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=hashes[0], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(),
        original_edge='Representative original XMS functions bound to released worker code',
        limits='Guest requests, A20-state pointer lifetime, UMB failures, reset and teardown remain unproved.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: representative bindings do not establish complete guest behavior', followup_stage='S12'))
assert len(proofs) == 4
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-0/xms-runtime-owner-review.md',
    'Four original XMS owner files partial; guest, error and teardown behavior remain open.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
              source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
              host_input_observations=[], scope='Representative selected bindings; not complete XMS behavior')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
