"""Reconcile relocated historical tool file roles without executing tools."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '3aab0fde4'
PREFIX = 'retained-tools16-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
observer = runpy.run_path(str(A / 'tools16-retained-observer.py'))
table = A / 'tools16-retained-observations.json'
observed = observer['main']()
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs = []
for entry in observed['entries']:
    path = entry['path']
    row, source = ledger[path], frozen[path]
    assert row['status'] in ('pending', 'excluded')
    assert entry['original_sha256'] == entry['retained_sha256'] == source['opennt45_sha256']
    assert not entry['dependencies']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=entry['retained_sha256'],
        edition='OpenNT-4.5', original_path=str(Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm') / path),
        prior_status='pending', status='excluded', role='historical-relocated-build-artifact-only',
        evidence_table='tools16-retained-observations.json',
        original_edge='Original mirror path absent; exact historical copy retained at ' + entry['retained_path'],
        limits='Only original mirror file role excluded; retained historical build tools and guest capabilities remain in scope.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: historical build artifact; retained binary, not runtime code',
        followup_stage='S11-S19'))
assert len(proofs) == 33
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'tools16-retained-role-review.md',
    'Thirty-three historical mirror tool roles excluded; exact relocated copies are retained.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
              source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
              host_input_observations=[], scope='Relocated historical file roles only; no guest capability exclusion')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
