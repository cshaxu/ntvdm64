"""Reconcile seven historical guest build-input file roles, not capabilities."""
import csv
import json
import runpy
from pathlib import Path

A = Path('artifacts/mvdm-audit')
BASE = '265172c44'
PREFIX = 'parallel-guest-resource-role-'
H = runpy.run_path(str(A / 'guest-source-role-checkpoint.py'))
contracts = {
    'wow16/ole/client/funchead.c': ('win16-oleclient-guest', 'Commented function template has no implementation', 'OLE entry points and object lifecycle remain open.'),
    'wow16/ole/client/mk.cmd': ('win16-oleclient-guest', 'Historical nmake build script', 'OLE runtime and actual image provenance remain open.'),
    'wow16/ole/client/utils.c': ('win16-oleclient-guest', 'Historical OLECLI guest build input referenced by its makefile', 'OLE utilities and guest object behavior remain open.'),
    'wow16/regedit/reg1632.h': ('win16-regedit-guest', 'Historical REGEDIT guest header included by regporte.c', 'Registry semantics, actual guest image and lifecycle remain open.'),
    'wow16/drivers/display/display.rc': ('win16-driver-resource', 'Historical display driver resource input', 'Guest driver image provenance and display behavior remain open.'),
    'wow16/drivers/display/DISPLAY.RES': ('win16-driver-resource', 'Historical display driver resource build artifact', 'Resource generation, image provenance and display behavior remain open.'),
    'wow16/drivers/keyboard/keyboard.rcv': ('win16-driver-resource', 'Historical keyboard driver resource input', 'Guest keyboard behavior and image provenance remain open.'),
}
tables = []
for group in sorted({v[0] for v in contracts.values()}):
    folder = A / 'shard-2'
    table = folder / (group + '-observations.json')
    assert runpy.run_path(str(folder / (group + '-observer.py'))).get('inspect')() == json.loads(table.read_text(encoding='utf-8'))
    tables.append(dict(path=table.relative_to(A).as_posix(), sha256=H['sha'](table)))
ledger = {r['relative_path']: r for r in H['rows'](A / 'combined-initial-status.csv')}
frozen = {r['relative_path']: r for r in H['rows'](A / 'frozen-file-reconciliation.csv')}
proofs = []
for path, (group, edge, limits) in contracts.items():
    row, source = ledger[path], frozen[path]
    assert row['status'] == 'pending' and row['shard'] == '2'
    assert source['current_sha256'] == source['opennt_sha256'] == source['opennt45_sha256']
    proofs.append(dict(relative_path=path, shard='2', sha256=source['current_sha256'], edition='OpenNT',
        original_path=str(Path('O:/repos.external/OpenNT/base/mvdm') / path), prior_status='pending', status='excluded',
        role='historical-guest-build-input-only', evidence_table='shard-2/' + group + '-observations.json',
        original_edge=edge, limits=limits, original_text_retained_percent='100', estimated_restored_code_percent='',
        restored_estimate_status='not-applicable: immutable historical guest build-input file role only', followup_stage='S12-S17'))
with (A / (PREFIX + 'dispositions.csv')).open('w', encoding='utf-8', newline='') as stream:
    writer = csv.DictWriter(stream, list(proofs[0]), lineterminator='\n'); writer.writeheader(); writer.writerows(proofs)
H['apply_dispositions'](proofs, BASE, PREFIX + 'review.md',
    'Seven original guest build-input file roles are excluded; all guest behavior and image provenance remain in scope.')
observation = dict(result='pass', base=BASE, candidate_count=len(proofs), source_tables=tables,
    host_input_observations=[], scope='Historical guest input file roles only; no guest runtime acceptance')
(A / (PREFIX + 'observations.json')).write_text(json.dumps(observation, indent=2) + '\n')
H['main'](A / (PREFIX + 'verification.json'), BASE, PREFIX, len(proofs))
