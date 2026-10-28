"""Reconcile two selected original WOW cross-language C declaration roles."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit'); BASE = '5617acf75'; PREFIX = 'wow-cross-language-header-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-1/wow-cross-language-constants-observations.json'; observed = json.loads(table.read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'shard-1/wow-cross-language-constants-observer.py'))['inspect']() == observed
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for item in observed['entries']:
    path = item['path']
    if path not in ('inc/wowcmpat.h', 'inc/wowusr.h'): continue
    row = ledger[path]; assert row['status'] == 'pending' and row['shard'] == '1'
    assert item['valid_dependency_matches']
    proofs.append(dict(relative_path=path, shard='1', sha256=item['sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(),
        original_edge='Selected original C header has normalized valid dependencies and corresponding original ASM constants',
        limits='Guest image selection, structure packing, table-slot semantics, API callbacks and lifecycle remain unproved.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: selected declarations and matching constants do not prove guest behavior', followup_stage='S14-S17'))
assert len(proofs) == 2
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, PREFIX + 'review.md',
    'Two selected WOW C headers remain partial; guest ASM, ABI and behavior remain in scope.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))], host_input_observations=[],
    scope='Selected C declarations and cross-language constants; no guest runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
