"""Integrate two selected original WOW bodies with bounded execution witnesses."""
import csv
import json
import runpy
import subprocess
import sys
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '250c1ace8', 'wow-selected-functions-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'cross-369/wow-selected-function-inventory.json'
observed = json.loads(table.read_text(encoding='utf-8-sig'))
assert observed == json.loads(subprocess.check_output([sys.executable, '-B', str(A / 'cross-369/wow-selected-function-inventory.py')]))
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for name in ('wgdi31', 'wkgthunk'):
    path = 'wow32/' + name + '.c'
    assert ledger[path]['status'] in ('pending', 'partial')
    original = Path('O:/repos.external/OpenNT/base/mvdm') / path
    assert H['sha'](original) == observed['files'][name]['source_sha256'].upper()
    proofs.append(dict(relative_path=path, shard=ledger[path]['shard'], sha256=H['sha'](original),
        edition='OpenNT', original_path=str(original), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(),
        original_edge='Original selected body and bounded execution witness; see cross-369/wow-selected-function-coverage.md',
        limits='Other entries, failure, reentry and retirement remain unverified; see the named per-function report',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: bounded witnesses do not measure full source behavior', followup_stage='S14-S16'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'cross-369/wow-selected-function-coverage.md',
                        'Two original WOW bodies have bounded execution evidence; remaining functionality is open.')
result = dict(result='pass', base=BASE, candidate_count=2,
              source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
              host_input_observations=[], scope='Bounded witnesses and function inventory; no whole-file runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 2)
