"""Reconcile selected shared headers and image-bound dependent objects."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '812e38e2e', 'mvdm-inc-header-owner-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
family_path = A / 'shard-0/mvdm-inc-header-family-observations.json'
owner_path = A / 'shard-0/mvdm-inc-header-owner-observations.json'
family = runpy.run_path(str(A / 'shard-0/mvdm-inc-header-family-observer.py'))['inspect']()
owner = runpy.run_path(str(A / 'shard-0/mvdm-inc-header-owner-observer.py'))['inspect']()
assert family == json.loads(family_path.read_text(encoding='utf-8-sig'))
assert owner == json.loads(owner_path.read_text(encoding='utf-8-sig'))
rows = {r['path']: r for r in family['files']}
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for item in owner['selected']:
    path, dependency = item['path'], item['dependency']
    source = rows[path]
    assert len(set(source['hashes'])) == 1 and dependency in source['valid_objects']
    assert ledger[path]['shard'] == '0' and ledger[path]['status'] in ('pending', 'partial')
    proofs.append(dict(relative_path=path, shard='0', sha256=source['hashes'][0], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=family_path.relative_to(A).as_posix(),
        original_edge=f"VALID dependency of {item['component']} object with a function body bound to its released image",
        limits='Conditional declarations, original service behavior, errors, identity and lifecycle remain unproved.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: header dependency is not behavioral coverage', followup_stage='S11-S17'))
assert len(proofs) == 15
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-0/mvdm-inc-header-family-review.md',
    'Fifteen selected shared declaration roles partial; other candidates and functional behavior remain open.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=p.relative_to(A).as_posix(), sha256=H['sha'](p)) for p in (family_path, owner_path)],
    host_input_observations=[], scope='Selected shared declarations, not guest or device acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
