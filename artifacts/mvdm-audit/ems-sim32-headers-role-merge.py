"""Reconcile selected EMS and Sim32 header file roles."""
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '7a6f51058', 'ems-sim32-headers-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
stem = 'shard-2/dos-emm-sim32-headers'
table = A / (stem + '-observations.json')
observed = json.loads(subprocess.check_output(['python', str(A / (stem + '-observer.py'))]))
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs = []
for item in observed['sources']:
    path = item['path']
    row, source = ledger[path], frozen[path]
    assert row['status'] in ('pending', 'partial')
    assert source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'],
        edition='OpenNT', original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path),
        prior_status='pending', status='partial', role='selected-native-source-partial',
        evidence_table=stem + '-observations.json', original_edge='Selected original EMS or Sim32 interface',
        limits='Guest mapping, pointer lifetime, reset, lease failure and teardown remain unproved.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: interface selection is not runtime coverage', followup_stage='S12'))
assert len(proofs) == 2
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-2/dos-emm-sim32-headers-review.md',
    'Two selected EMS and Sim32 declarations partial; runtime behavior remains open.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
              source_tables=[dict(path=stem + '-observations.json', sha256=H['sha'](table))],
              host_input_observations=[], scope='Selected interfaces, not complete guest behavior')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
