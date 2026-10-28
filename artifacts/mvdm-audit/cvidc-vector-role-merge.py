"""Record the selected original C-VID vector table as partial."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = '27a9d2e63', 'cvidc-vector-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
observed = json.loads((A / 'cvidc-vector-image-observations.json').read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'cvidc-vector-image-observer.py'))['inspect']() == observed
row = next(r for r in H['rows'](A / 'combined-initial-status.csv') if r['relative_path'] == observed['relative_path'])
assert row['status'] == 'pending'
proofs = [dict(relative_path=row['relative_path'], shard=row['shard'], sha256=observed['sha256'],
    edition='OpenNT', original_path=observed['original_path'], prior_status='pending', status='partial',
    role='selected-native-source-partial', evidence_table='cvidc-vector-image-observations.json',
    original_edge='Fourteen original read/write/mark tables bind 692 image pointers to original generated owners',
    limits='Prove dynamic mode selection, table publication, plane and latch behavior, bounds and retirement.',
    original_text_retained_percent='100', estimated_restored_code_percent='',
    restored_estimate_status='unmeasurable: static vector binding does not establish complete execution or lifetime behavior',
    followup_stage='S11/S13')]
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'cvidc-vector-role-review.md',
    'Original C-VID vector tables bind 692 initial image pointers; dynamic selection and behavior remain open.')
observation = dict(result='pass', base=BASE, candidate_count=1,
    source_tables=[dict(path='cvidc-vector-image-observations.json', sha256=H['sha'](A / 'cvidc-vector-image-observations.json'))],
    host_input_observations=[], scope='Initial linked vector values only, not runtime mode or plane behavior')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, 1)
