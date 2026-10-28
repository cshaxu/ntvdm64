"""Reconcile selected C-VID core roles with bounded released-image evidence."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'bc6bdee9f', 'cvidc-selected-core-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-2/cvidc-selected-core-image-observations.json'
observed = runpy.run_path(str(A / 'shard-2/cvidc-selected-core-image-observer.py'))['inspect']()
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs = []
for item in observed['sources']:
    path, hashes = item['path'], item['hashes']
    row, source = ledger[path], frozen[path]
    assert row['status'] in ('pending', 'partial') and row['shard'] == '2'
    assert hashes[0] == hashes[1] == hashes[2] == source['current_sha256']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=hashes[0], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(),
        original_edge='Original selected object and representative relocation-masked release code binding',
        limits='Complete dispatch, relocation targets, rendering, state ownership and teardown remain unproved.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: sample binding does not establish full guest behavior', followup_stage='S13'))
assert len(proofs) == 3
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-2/cvidc-selected-core-image-review.md',
    'Three selected C-VID owner roles partial; complete video behavior remains open.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
              source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
              host_input_observations=[], scope='Representative relocation-masked bindings, not complete rendering proof')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
