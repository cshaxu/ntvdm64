"""Reconcile original empty declaration file roles without excluding features."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '9885952a4'
PREFIX = 'parallel-empty-declaration-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/cvidc-empty-guards-observations.json'
observed = json.loads(table.read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'shard-0/cvidc-empty-guards-observer.py'))['inspect']() == observed
assert len(observed['sources']) == 11
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for source in observed['sources']:
    path = source['relative_path']
    row = ledger[path]
    assert row['status'] == 'pending'
    assert row['shard'] == ('3' if path == 'inc/APISTRUC.INC' else '0')
    edge = 'Zero-byte original generated include' if path == 'inc/APISTRUC.INC' else 'Original header contains only an empty include guard'
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='excluded',
        role='historical-empty-declaration-only', evidence_table=table.relative_to(A).as_posix(), original_edge=edge,
        limits='This physical empty file role excludes no CPU, video, network, guest, callback or lifecycle capability.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: original file contains no behavioral declarations or implementation', followup_stage='S12/S13'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-0/cvidc-empty-guards-review.md',
    'Eleven original empty declaration file roles are excluded; CPU, video and network capabilities remain in scope.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='Empty physical declaration roles only; no capability exclusion')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
