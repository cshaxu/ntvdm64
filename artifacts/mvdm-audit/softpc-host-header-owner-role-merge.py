"""Reconcile selected host header dependencies without asserting behavior."""
import csv
import json
import runpy
import subprocess
import sys
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '1ff330f9f', 'softpc-host-header-owner-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
family_path = A / 'shard-1/softpc-host-header-family-evidence.json'
owner_path = A / 'shard-1/softpc-host-header-owner-observations.json'
family = json.loads(subprocess.check_output([sys.executable, '-B', str(A / 'shard-1/softpc-host-header-family-observer.py')]))
owner = runpy.run_path(str(A / 'shard-1/softpc-host-header-owner-observer.py'))['inspect']()
assert family == json.loads(family_path.read_text(encoding='utf-8-sig'))
assert owner == json.loads(owner_path.read_text(encoding='utf-8-sig'))
rows = {r['name']: r for r in family['files']}
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for name, dependency in owner['dependencies'].items():
    source, path = rows[name], 'softpc.new/host/inc/' + name
    assert source['reference_hashes'] == [source['sha256'], source['sha256']]
    assert dependency in source['valid_worker_objects']
    assert ledger[path]['shard'] == '1' and ledger[path]['status'] in ('pending', 'partial')
    proofs.append(dict(relative_path=path, shard='1', sha256=source['sha256'].upper(), edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=family_path.relative_to(A).as_posix(),
        original_edge='VALID dependency of selected host object with function body bound to released worker',
        limits='Conditional declarations, device operations, failures, state ownership and lifecycle remain unproved.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: selected header dependency does not establish behavior', followup_stage='S12-S13'))
assert len(proofs) == 4
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-1/softpc-host-header-family-review.md',
    'Four selected original host declaration roles partial; device and lifecycle behavior remain open.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=p.relative_to(A).as_posix(), sha256=H['sha'](p)) for p in (family_path, owner_path)],
    host_input_observations=[], scope='Selected host declarations, not device acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
