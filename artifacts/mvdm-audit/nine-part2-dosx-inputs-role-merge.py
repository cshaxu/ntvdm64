"""Reconcile part-2 original DOSX x86 build input file roles."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '74ce4d62a', 'nine-part2-dosx-inputs-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'nine-part2-dosx-inputs-observations.json'
observed = runpy.run_path(str(A / 'nine-part2-dosx-inputs-observer.py'))['inspect']()
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
owned = {r['key'] for r in H['rows'](A / 'nine-way-ownership.csv') if r['kind'] == 'mvdm' and r['bucket'] == '2'}
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for source in observed['files']:
    path = source['path']
    assert path in owned and len(set(source['hashes'])) == 1
    assert ledger[path]['status'] in ('pending', 'excluded')
    proofs.append(dict(relative_path=path, shard=ledger[path]['shard'], sha256=source['hashes'][0], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='excluded',
        role='historical-guest-build-input-only', evidence_table=table.relative_to(A).as_posix(),
        original_edge='Original DOSX x86 makefile assembles this source and the response file selects its object',
        limits='Historical file role only; actual DOSX image lineage, protected-mode behavior, failures and retirement remain open.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: immutable guest build input, not DOSX behavior', followup_stage='S12'))
assert len(proofs) == 7
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'nine-way/part-2/dpmi-guest-family-review.md',
    'Seven original part-2 DOSX build-file roles excluded; protected-mode behavior remains in scope.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='DOSX guest build input file roles only')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
