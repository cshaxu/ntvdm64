"""Reconcile exact original guest package identities, not runtime behavior."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'ddc8fc533', 'guest-package-image-family-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/guest-package-image-family-observations.json'
observed = runpy.run_path(str(A / 'shard-0/guest-package-image-family-observer.py'))['inspect']()
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for source in observed['files']:
    path = source['path']
    assert len(set(source['hashes'])) == 1
    assert ledger[path]['shard'] == '0' and ledger[path]['status'] in ('pending', 'partial')
    installed = 'and installed exact path' if source['installed_sha256'] else 'but installed exact path was absent'
    proofs.append(dict(relative_path=path, shard='0', sha256=source['hashes'][0], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-guest-image-partial', evidence_table=table.relative_to(A).as_posix(),
        original_edge=f'Original guest binary matches archive {installed}',
        limits='Guest load path, source-to-image provenance, actual service behavior and lifecycle remain unproved.',
        original_text_retained_percent='', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: guest image identity does not establish behavior', followup_stage='S12-S17'))
assert len(proofs) == 5
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-0/guest-package-image-family-review.md',
    'Five original guest images match the archive; actual loading and runtime behavior remain open.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='Guest image identity, not execution acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
