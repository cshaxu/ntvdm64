"""Reconcile original DOSX x86 guest build roles, not DOSX behavior."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '5eb969391', 'dosx-x86-inputs-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/dosx-x86-inputs-observations.json'
observed = runpy.run_path(str(A / 'shard-0/dosx-x86-inputs-observer.py'))['inspect']()
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for source in observed['files']:
    path = source['path']
    assert len(set(source['hashes'])) == 1 and ledger[path]['shard'] == '0'
    assert ledger[path]['status'] in ('pending', 'excluded')
    proofs.append(dict(relative_path=path, shard='0', sha256=source['hashes'][0], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='excluded',
        role='historical-guest-build-input-only', evidence_table=table.relative_to(A).as_posix(),
        original_edge='Original DOSX x86 guest makefile and response select this assembler input or include',
        limits='Historical file role only; alternate profiles, source-to-image lineage, DPMI behavior and lifecycle remain open.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: immutable guest build input, not DOSX behavior', followup_stage='S12'))
assert len(proofs) == 12
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-0/dosx-x86-inputs-review.md',
    'Twelve original DOSX guest build-file roles excluded; protected-mode behavior remains in scope.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='Original guest build inputs only; no DOSX runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
