"""Reconcile two guest images and their historical intermediate."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit'); BASE = '145327a87'; PREFIX = 'dos-kernel-image-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/dos-kernel-image-observations.json'; observed = json.loads(table.read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'shard-0/dos-kernel-image-observer.py'))['inspect']() == observed
assert len(observed['sources']) == 3 and len(observed['images']) == 2
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for item in observed['sources']:
    path = item['path']; row = ledger[path]; assert row['status'] == 'pending' and row['shard'] == '0'
    intermediate = path.endswith('NTDOS.BIN')
    proofs.append(dict(relative_path=path, shard='0', sha256=item['sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending',
        status='excluded' if intermediate else 'partial', role='historical-guest-build-input-only' if intermediate else 'selected-guest-image-partial',
        evidence_table=table.relative_to(A).as_posix(),
        original_edge='Historical DOS kernel relocation intermediate' if intermediate else 'Original guest image matches archive and installation',
        limits='Guest execution, source-to-image provenance, DOS services, error and lifecycle behavior remain open.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: historical guest build intermediate only' if intermediate else 'unmeasurable: image identity does not establish runtime behavior', followup_stage='S12-S13'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-0/dos-kernel-image-review.md',
    'Two guest images remain partial; one historical kernel intermediate file role is excluded.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))], host_input_observations=[],
    scope='Guest image identities and intermediate role only; no guest runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
