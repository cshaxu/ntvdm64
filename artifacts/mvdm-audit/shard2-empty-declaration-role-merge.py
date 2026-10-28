"""Reconcile eleven empty C-VID header roles, not their capabilities."""
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '0c9b9bdba'
PREFIX = 'shard2-empty-declaration-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
stems = ('shard-2/cvidc-header-batch', 'shard-2/cvidc-guard-closure')
tables = []
for stem in stems:
    path = A / (stem + '-observations.json')
    live = json.loads(subprocess.check_output(['python', str(A / (stem + '-observer.py'))]))
    saved = json.loads(path.read_text(encoding='utf-8-sig'))
    assert live == saved
    if stem.endswith('guard-closure'):
        assert len(live['headers']) == 11 and not live['non_guard_hits'] and not live['filename_hits']
        headers = live['headers']
    tables.append(dict(path=stem + '-observations.json', sha256=H['sha'](path)))
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs = []
for header in headers:
    path = 'softpc.new/base/cvidc/' + header['name']
    row, source = ledger[path], frozen[path]
    assert row['status'] in ('pending', 'excluded')
    assert source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256'] == header['sha256']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=source['current_sha256'],
        edition='OpenNT', original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path),
        prior_status='pending', status='excluded', role='historical-empty-declaration-only',
        evidence_table=stems[1] + '-observations.json', original_edge='Guard-only file with no observed guard consumer',
        limits='File role only; CPU/video features and noncaptured profiles remain in scope.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: declaration-less file role; no feature excluded', followup_stage='S11-S13'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-2/cvidc-guard-closure-review.md',
    'Eleven declaration-less header roles excluded; CPU and video capabilities remain open.')
result = dict(result='pass', base=BASE, candidate_count=len(proofs), source_tables=tables,
              host_input_observations=[], scope='Empty header roles only; no runtime feature exclusion')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(result, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
