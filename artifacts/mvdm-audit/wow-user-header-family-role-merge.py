"""Classify selected WOW declarations, not their runtime behavior."""
import csv
import json
import runpy
import subprocess
import sys
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '6d0844bac', 'wow-user-header-family-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-1/wow-user-header-family-evidence.json'
observed = json.loads(subprocess.check_output([sys.executable, '-B', str(A / 'shard-1/wow-user-header-family-observer.py')]))
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for source in observed['files']:
    if not source['valid_dependency_objects']:
        continue
    path = 'wow32/' + source['name']
    assert source['all_equal'] and source['direct_includes']
    assert ledger[path]['status'] in ('pending', 'partial') and ledger[path]['shard'] == '1'
    proofs.append(dict(relative_path=path, shard='1', sha256=source['sha256'].upper(), edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(),
        original_edge=f"Original declaration is included by selected WOW32 sources; {len(source['valid_dependency_objects'])} current VALID object dependencies",
        limits='Selected declaration does not prove each message, callback, failure, object lifetime or guest behavior.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: selected declaration identity does not establish behavior', followup_stage='S14-S15'))
assert len(proofs) == 11
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-1/wow-user-header-family-review.md',
    'Eleven selected WOW USER declarations partial; two unselected candidates remain pending.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='Selected WOW declarations, not guest runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
