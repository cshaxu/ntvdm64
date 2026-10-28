"""Classify two selected CCPU instruction leaves as partial."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = 'f56fd3f3e'
PREFIX = 'shard0-bit-rotate-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/ccpu-bit-rotate-image-observations.json'
observed = json.loads(table.read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'shard-0/ccpu-bit-rotate-image-observer.py'))['inspect']() == observed
assert {item['path'] for item in observed['sources']} == {'softpc.new/base/ccpu386/btc.c', 'softpc.new/base/ccpu386/rcl.c'}
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for item in observed['sources']:
    path = item['path']; row = ledger[path]
    assert row['status'] == 'pending' and row['shard'] == '0'
    proofs.append(dict(relative_path=path, shard='0', sha256=item['sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(),
        original_edge='Original CCPU40 instruction body selected and bound to released NTVDM',
        limits='Guest operand, flags, memory, faults and lifetime contracts require separate S11 validation.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: identical text and bound body do not establish instruction behavior', followup_stage='S11'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-0/ccpu-bit-rotate-image-review.md',
    'BTC and RCL selected source bodies remain partial; full guest semantics remain in scope.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='Selected instruction source and image binding; no complete guest acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
