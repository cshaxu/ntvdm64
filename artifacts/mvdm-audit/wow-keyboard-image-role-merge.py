"""Reconcile the selected keyboard thunk owner with released code evidence."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '139555283', 'wow-keyboard-image-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/wow-keyboard-image-observations.json'
observed = runpy.run_path(str(A / 'shard-0/wow-keyboard-image-observer.py'))['inspect']()
assert observed == json.loads(table.read_text(encoding='utf-8-sig'))
path = observed['source']
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
assert ledger[path]['status'] in ('pending', 'partial') and len(observed['bindings']) == 11
assert observed['hashes'][0] == observed['hashes'][1] == observed['hashes'][2]
proof = dict(relative_path=path, shard=ledger[path]['shard'], sha256=observed['hashes'][0], edition='OpenNT',
    original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
    role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(),
    original_edge='Eleven original keyboard thunk bodies bind to released WOW32 executable code',
    limits='Guest buffers, selector bounds, dead keys, layouts, errors and task teardown remain unproved.',
    original_text_retained_percent='100', estimated_restored_code_percent='',
    restored_estimate_status='unmeasurable: body binding does not establish guest behavior', followup_stage='S15')
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proof), lineterminator='\n')
    writer.writeheader()
    writer.writerow(proof)
H['apply_dispositions']([proof], BASE, 'shard-3/wow-keyboard-thunks-lifecycle-review.md',
    'Selected WOW keyboard thunk owner partial; guest and lifecycle behavior remain open.')
result = dict(result='pass', base=BASE, candidate_count=1,
              source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
              host_input_observations=[], scope='Keyboard thunk body binding, not guest behavior acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 1)
