"""Reconcile selected original multimedia and dispatch declaration roles."""
import csv
import json
import runpy
import subprocess
from pathlib import Path

A = Path('artifacts/mvdm-audit'); BASE = '381203570'; PREFIX = 'wow-media-dispatch-header-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
groups = ('wow32-media-message', 'wow32-dispatch-header'); tables = []; observed = {}
for name in groups:
    table = A / 'shard-2' / (name + '-observations.json')
    actual = runpy.run_path(str(A / 'shard-2' / (name + '-observer.py')))['inspect']()
    assert actual == json.loads(table.read_text(encoding='utf-8')); observed[name] = actual
    tables.append(dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table)))
deps = subprocess.check_output(['ninja', '-C', 'build/.ninja/msvc-x86-release/wow32', '-t', 'deps'], text=True, encoding='utf-8', errors='replace').replace('\\', '/').lower()
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
items = observed['wow32-media-message']['sources'] + [x for x in observed['wow32-dispatch-header']['sources'] if x['path'] == 'wow32/wowtbl.h']
proofs = []
for item in items:
    path = item['path']; row = ledger[path]; assert row['status'] == 'pending' and row['shard'] == '2'
    assert item['hashes'][0] == item['hashes'][1] == item['hashes'][2]
    assert '/src/mvdm/' + path.lower() in deps
    table = 'shard-2/wow32-dispatch-header-observations.json' if path.endswith('wowtbl.h') else 'shard-2/wow32-media-message-observations.json'
    proofs.append(dict(relative_path=path, shard='2', sha256=item['hashes'][0], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='partial',
        role='selected-native-source-partial', evidence_table=table,
        original_edge='Original declaration or table included in selected WOW32 compilation',
        limits='Guest ABI, table relocation, callback, errors, object lifetime and actual invocation remain unproved.',
        original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: source and dependency identity do not establish guest behavior', followup_stage='S14-S17'))
assert len(proofs) == 4
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, PREFIX + 'review.md',
    'Four original multimedia and dispatch declaration roles remain partial; guest execution remains in scope.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs), source_tables=tables, host_input_observations=[],
    scope='Selected original declaration dependencies only; no guest runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
