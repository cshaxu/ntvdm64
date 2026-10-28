"""Apply partial dispositions for selected generated C-VID objects."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '626f77507', 'cvidc-generated-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
observed = json.loads((A / 'cvidc-generated-image-observations.json').read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'cvidc-generated-image-observer.py'))['inspect']() == observed
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for row in observed['sources']:
    assert ledger[row['relative_path']]['status'] == 'pending'
    first = row['functions'][0]['name']
    proofs.append(dict(relative_path=row['relative_path'], shard=ledger[row['relative_path']]['shard'],
        sha256=row['sha256'], edition='OpenNT', original_path=row['original_path'],
        prior_status='pending', status='partial', role='selected-native-source-partial',
        evidence_table='cvidc-generated-image-observations.json',
        original_edge=f"Selected C-VID object binds 33 image functions including {first}; {len(row['table_names'])} names occur in evidfunc.c",
        limits='Prove actual mode dispatch, plane/latch data, bounds, aliasing and reset; relocation-masked binding and source mentions do not prove behavior.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: original source retained; per-mode execution and lifecycle remain unproved',
        followup_stage='S11/S13'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'cvidc-generated-role-review.md',
    'Seventeen selected generated C-VID units are partial; video data semantics remain open.')
observation = dict(result='pass', base=BASE, candidate_count=17,
    source_tables=[dict(path='cvidc-generated-image-observations.json', sha256=H['sha'](A / 'cvidc-generated-image-observations.json'))],
    host_input_observations=[], scope='Image code and table source references, not mode execution')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 17)
