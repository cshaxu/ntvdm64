"""Reconcile selected original WOW header identities and graph dependencies."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '3b589a920', 'wow-core-header-family-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/wow-header-family-observations.json'
observed = runpy.run_path(str(A / 'shard-0/wow-header-family-observer.py'))['inspect']()
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for source in observed['files']:
    if not source['valid_dependency_objects']:
        continue
    path = source['path']
    assert len(set(source['hashes'])) == 1 and source['direct_includes']
    assert ledger[path]['status'] in ('pending', 'partial') and ledger[path]['shard'] == '0'
    proofs.append(dict(relative_path=path, shard='0', sha256=source['hashes'][0], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(),
        original_edge=f"Original declaration included in selected WOW32 source; {len(source['valid_dependency_objects'])} VALID object dependencies",
        limits='Precompiled-header dependencies do not prove all declared messages, dispatch, callbacks or guest behavior.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: header build identity does not establish behavior', followup_stage='S14-S16'))
assert len(proofs) == 17
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-0/wow-header-family-review.md',
    'Seventeen selected WOW declaration roles partial; four unselected candidates remain pending.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='Selected WOW declarations, not guest runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
