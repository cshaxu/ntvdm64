"""Merge three independently replayed original WOW GDI roles."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE, PREFIX = 'ef8c75472', 'parallel-wow-gdi-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
path = 'shard-2/parallel-review-third-evidence.json'
observed = json.loads((A / path).read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'shard-2/parallel-review-third-observer.py'))['inspect']() == observed
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
limits = {
    'wgfont.c': 'Font resource failures, guest enumeration callbacks, reentry and retirement remain open.',
    'wgtext.c': 'Guest strings, encoding, clipping, copyback, failures and reentry remain open.',
    'wgmeta.c': 'Metafile record and handle ownership, callbacks, failures and teardown remain open.',
}
proofs = []
for source in observed['sources']:
    name = source['name']; relative = 'wow32/' + name
    if name not in limits:
        continue
    row, f = ledger[relative], frozen[relative]
    assert row['status'] == 'pending' and source['sha256'] == f['current_sha256'] == f['opennt_sha256'] == f['opennt45_sha256']
    owner = next(o for o in observed['owners'] if o['owner'] == name[:-2])
    proofs.append(dict(relative_path=relative, shard=row['shard'], sha256=source['sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / relative),
        prior_status='pending', status='partial', role='selected-native-source-partial', evidence_table=path,
        original_edge=f"Original GDI thunk owner resolves {len(owner['entries'])} linked dispatch ordinals",
        limits=limits[name], original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: bound initial table and code do not prove guest behavior or lifecycle',
        followup_stage='S14/S16'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-2/parallel-review-third.md',
    'Three original WOW GDI owners bind 28 initial dispatch targets; guest behavior and callbacks remain open.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=path, sha256=H['sha'](A / path))], host_input_observations=[],
    scope='Initial GDI dispatch and code binding only, not guest behavior acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
