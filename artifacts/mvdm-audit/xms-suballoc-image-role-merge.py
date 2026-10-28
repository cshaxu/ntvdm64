"""Classify selected original XMS and suballocator owners as partial."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit'); BASE = 'fef0cf799'; PREFIX = 'xms-suballoc-image-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
source_table = A / 'shard-2/xms-suballoc-roles-observations.json'
image_table = A / 'shard-0/xms-suballoc-image-observations.json'
source = json.loads(source_table.read_text(encoding='utf-8')); image = json.loads(image_table.read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'shard-2/xms-suballoc-roles-observer.py'))['inspect']() == source
assert runpy.run_path(str(A / 'shard-0/xms-suballoc-image-observer.py'))['inspect']() == image
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for item in source['sources']:
    path = item['path']
    if path not in ('xms.486/xmsdisp.c', 'suballoc/suballoc.c'): continue
    row = ledger[path]; assert row['status'] == 'pending' and row['shard'] == '2'
    assert item['hashes'][0] == item['hashes'][1] == item['hashes'][2]
    proofs.append(dict(relative_path=path, shard='2', sha256=item['hashes'][0], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=source_table.relative_to(A).as_posix(),
        original_edge='Original selected source and representative object bodies bind to released NTVDM',
        limits='Real guest requests, index bounds, allocation failure, rollback, reentry and teardown remain unproved.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: selected source and image binding do not establish guest behavior', followup_stage='S12'))
assert len(proofs) == 2
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-0/xms-suballoc-image-review.md',
    'Original XMS dispatch and suballocator owners remain partial; guest and failure behavior remain in scope.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=p.relative_to(A).as_posix(), sha256=H['sha'](p)) for p in (source_table, image_table)],
    host_input_observations=[], scope='Source and image binding only; no guest runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
