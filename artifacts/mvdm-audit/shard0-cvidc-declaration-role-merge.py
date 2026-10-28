"""Reconcile selected C-VID declarations without claiming runtime semantics."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '09265ef54'
PREFIX = 'shard0-cvidc-declaration-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
table = A / 'shard-0/cvidc-declaration-observations.json'
observed = json.loads(table.read_text(encoding='utf-8'))
assert runpy.run_path(str(A / 'shard-0/cvidc-declaration-observer.py'))['inspect']() == observed
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
limits = {
    'cpu4gen.h': 'CPU vector ABI, state, mode selection, nesting and reset remain open.',
    'evid_c.h': 'Video state layout, mode selection, transitions and lifecycle remain open.',
    'gdpvar.h': 'Actual global state ownership, update ordering and teardown remain open.',
    'univer_c.h': 'The actual non-PROD accessfn context does not prove runtime execution, ABI or CPU state behavior.',
}
proofs = []
for record in observed['sources']:
    path = record['relative_path']
    row, original = ledger[path], frozen[path]
    assert row['status'] == 'pending' and row['shard'] == '0'
    assert original['current_sha256'] == original['opennt_sha256'] == original['opennt45_sha256'] == record['sha256']
    proofs.append(dict(relative_path=path, shard=row['shard'], sha256=record['sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending',
        status='partial', role='selected-native-source-partial', evidence_table=table.relative_to(A).as_posix(),
        original_edge='Original declarations selected by the actual compiler context for ' + record['object'],
        limits=limits[Path(path).name], original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='unmeasurable: selected declarations do not prove ABI or execution', followup_stage='S13'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n')
    writer.writeheader()
    writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, 'shard-0/cvidc-declaration-review.md',
    'Four C-VID declaration owners are partial; ABI, behavior and lifecycle remain open.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs),
    source_tables=[dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table))],
    host_input_observations=[], scope='Compiler-selected declaration evidence only; no runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
