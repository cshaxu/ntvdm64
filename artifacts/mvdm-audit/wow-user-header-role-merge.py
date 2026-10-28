"""Reconcile selected original WOW32 USER declaration file roles."""
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit'); BASE = 'e5264914b'; PREFIX = 'wow-user-header-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-2/wow32-user-header-observations.json'; observed = json.loads(table.read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'shard-2/wow32-user-header-observer.py'))['inspect']() == observed
deps = subprocess.check_output(['ninja', '-C', 'build/.ninja/msvc-x86-release/wow32', '-t', 'deps'], text=True, encoding='utf-8', errors='replace').replace('\\', '/').lower()
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
proofs = []
for item in observed['sources']:
    path = item['path']; row = ledger[path]; assert row['status'] == 'pending' and row['shard'] == '2'
    assert item['hashes'][0] == item['hashes'][1] == item['hashes'][2]
    assert '/src/mvdm/' + path.lower() in deps
    proofs.append(dict(relative_path=path, shard='2', sha256=item['hashes'][0], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(),
        original_edge='Original declaration header included by selected WOW32 compilation; representative owner in map',
        limits='Actual table relocation, guest call, all declarations, callbacks, failures and lifecycle remain unproved.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: selected declarations and map samples do not establish runtime coverage', followup_stage='S14-S17'))
assert len(proofs) == 5
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, PREFIX + 'review.md',
    'Five selected original USER declaration roles remain partial; guest behavior remains in scope.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))], host_input_observations=[],
    scope='Original declarations and selected dependency evidence only; no guest runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
